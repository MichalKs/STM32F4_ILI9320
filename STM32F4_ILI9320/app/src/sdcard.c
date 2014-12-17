/**
 * @file:   sdcard.c
 * @brief:  SD card control functions.
 * @date:   22 kwi 2014
 * @author: Michal Ksiezopolski
 * 
 * @verbatim
 * Copyright (c) 2014 Michal Ksiezopolski.
 * All rights reserved. This program and the 
 * accompanying materials are made available 
 * under the terms of the GNU Public License 
 * v3.0 which accompanies this distribution, 
 * and is available at 
 * http://www.gnu.org/licenses/gpl.html
 * @endverbatim
 */

#include <sdcard.h>
#include <spi1.h>
#include <timers.h>
#include <stdio.h>

/**
 * @addtogroup SD_CARD
 * @{
 */

#ifndef DEBUG
  #define DEBUG
#endif

#ifdef DEBUG
  #define print(str, args...) printf(""str"%s",##args,"")
  #define println(str, args...) printf("SD--> "str"%s",##args,"\r\n")
#else
  #define print(str, args...) (void)0
  #define println(str, args...) (void)0
#endif

/*
 * SD commands (SPI command subset) as per SanDisk Secure Digital Card product manual.
 * TODO Add all commands
 */
#define SD_GO_IDLE_STATE            0   ///< Resets SD Card.
#define SD_SEND_OP_COND             1   ///< Activates the card initialization process, sends host capacity.
#define SD_SEND_IF_COND             8   ///< Asks card whether it can operate in given voltage range.
#define SD_SEND_CSD                 9   ///< Ask for card specific data (CSD).
#define SD_SEND_CID                 10  ///< Ask for card identification (CID).
#define SD_STOP_TRANSMISSION        12  ///< Forces a card to stop transmission during a multiple block read operation.
#define SD_SEND_STATUS              13  ///< Ask for status register contents.
#define SD_SET_BLOCKLEN             16  ///< Selects block length in bytes for all following block commands
#define SD_READ_SINGLE_BLOCK        17  ///< Reads a block of size set by SET_BLOCKLEN
#define SD_READ_MULTIPLE_BLOCK      18  ///< Continuously transfers data blocks from card to host until interrupted by STOP_TRANSMISSION
#define SD_WRITE_BLOCK              24  ///< Writes a block of size set by SET_BLOCKLEN
#define SD_WRITE_MULTIPLE_BLOCK     25  ///< Continuously writes blocks of data until a stop transmission token is sent
#define SD_PROGRAM_CSD              27  ///< Programs the programmable bits of CSD
#define SD_ERASE_WR_BLK_START_ADDR  32  ///< Sets the address of the first write block to be erased
#define SD_ERASE_WR_BLK_END_ADDR    33  ///< Sets the address of the last write block of the continuous range to be erased
#define SD_ERASE                    38  ///< Erases all previously selected write blocks
#define SD_APP_CMD                  55  ///< Next command is application specific command
#define SD_READ_OCR                 58  ///< Reads OCR register
#define SD_CRC_ON_OFF               59  ///< Turns CRC on or off
/*
 * Application specific commands, ACMD
 */
#define SD_ACMD_SEND_OP_COND        41  ///< Activates the card initialization process, sends host capacity.
#define SD_ACMD_SEND_SCR            51  ///< Reads SD Configuration register

/*
 * Other SD defines
 */
#define SD_IF_COND_CHECK  0xaa    ///< Check pattern for SEND_IF_COND command
#define SD_IF_COND_VOLT   (1<<8)  ///< Signifies voltage range 2.7-3.6V
#define SD_ACMD41_HCS     (1<<30) ///< Host can handle SDSC and SDHC cards

/*
 * Control tokens
 */

#define SD_TOKEN_SBR_MBR_SBW  0xfe ///< Start block for single block read, multiple block read, single block write. This token is sent, then 2-513 bytes of data, two bytes CRC
#define SD_TOKEN_MBW_START    0xfc ///< Start block token for multiple block write - data will be transferred
#define SD_TOKEN_MBW_STOP     0xfd ///< Stop transmission token for multiple block write

/*
 * Every data block sent to SD card will be acknowledged by data response token.
 * In case of error during Multiple Block Write host shall stop transmission
 * using CMD12. ACMD22 may be used to find number of well written blocks.
 * CMD13 may be sent to get cause of write problem
 */
#define SD_TOKEN_DATA_ACCEPTED  0x05 ///< Data accepted
#define SD_TOKEN_DATA_CRC       0x0b ///< Data rejected due to CRC error
#define SD_TOKEN_DATA_WRITE_ERR 0x0d ///< Data rejected due to write error

static uint8_t SD_SendCommand(uint8_t cmd, uint32_t args);
static void SD_GetResponseR3orR7(uint8_t* buf);

#define SD_HAL_Init SPI1_Init
#define SD_HAL_SelectCard SPI1_Select
#define SD_HAL_DeselectCard SPI1_Deselect
#define SD_HAL_TransmitData SPI1_Transmit
#define SD_HAL_ReadBuffer SPI1_ReadBuffer

static uint8_t isSDHC; ///< Is the card SDHC?

/**
 * @brief SD Card R1 response structure
 * @details This token is sent after every command
 * with the exception of SEND_STATUS command
 */
typedef union {

  struct {
    uint8_t inIdleState         :1; ///< The card is in IDLE state
    uint8_t eraseReset          :1; ///< Erase sequence was cleared before executing because an out of erase sequence commands was received
    uint8_t illegalCommand      :1; ///< Illegal command code detected
    uint8_t commErrorCRC        :1; ///< CRC check of last command failed
    uint8_t eraseSequenceError  :1; ///< Error in sequence of erase commands
    uint8_t addressErrror       :1; ///< Misaligned address didn't match block length used in command
    uint8_t parameterError      :1; ///< Command's argument was outside the range allowed for the card
    uint8_t reserved            :1; ///< Reserved (always 0)
  } flags;

  uint8_t responseR1; ///< R1 response fields as byte
} SD_ResponseR1;

/**
 * @brief SD Card R2 response structure
 * @details This token is sent in response to SEND_STATUS command.
 *
 */
typedef union {

  struct {
    uint16_t cardLocked          :1; ///< Set when card is locked bu user
    uint16_t wpEraseSkip         :1; ///< Set when host attempts to write a write-protected sector or makes errors during card lock/unlock operation
    uint16_t error               :1; ///< General or unknown error occured
    uint16_t errorCC             :1; ///< Internal card controller error
    uint16_t cardFailedECC       :1; ///< Card internal ECC was applied but failed to correct data
    uint16_t wpViolation         :1; ///< Command tried to write a write-protected block
    uint16_t eraseParam          :1; ///< Invalid selection for erase, sectors, groups.
    uint16_t outOfRange          :1; ///<
    uint16_t inIdleState         :1; ///< The card is in IDLE state
    uint16_t eraseReset          :1; ///< Erase sequence was cleared before executing because an out of erase sequence commands was received
    uint16_t illegalCommand      :1; ///< Illegal command code detected
    uint16_t commErrorCRC        :1; ///< CRC check of last command failed
    uint16_t eraseSequenceError  :1; ///< Error in sequence of erase commands
    uint16_t addressErrror       :1; ///< Misaligned address didn't match block length used in command
    uint16_t parameterError      :1; ///< Command's argument was outside the range allowed for the card
    uint16_t reserved            :1; ///< Reserved (always 0)
  } flags;

  uint16_t responseR2; ///< R1 response fields as byte

} SD_ResponseR2;

/**
 * @brief OCR register
 */
typedef union {

  struct {
    uint32_t reserved   :15;
    uint32_t volt27to28 :1;
    uint32_t volt28to29 :1;
    uint32_t volt29to30 :1;
    uint32_t volt30to31 :1;
    uint32_t volt31to32 :1;
    uint32_t volt32to33 :1;
    uint32_t volt33to34 :1;
    uint32_t volt34to35 :1;
    uint32_t volt35to36 :1;
    uint32_t switchingTo18      :1;
    uint32_t reserved2          :5;
    uint32_t cardCapacityStatus :1; ///< 0 - SDSC, 1 - SDHC, valid only after power up bit is 1
    uint32_t cardPowerUpStatus  :1; ///< Set to 0 if card has not finished power up routine
  } bits;

  uint32_t ocr;

} SD_OCR;

/**
 * @brief Initialize the SD card.
 *
 * @details This function initializes both SDSC and SDHC cards.
 * It uses low-level SPI functions.
 *
 */
void SD_Init(void) {

  int i; // for counter
  uint8_t buf[10]; // buffer for responses

  SD_HAL_Init(); // Initialize SPI interface.

  SD_HAL_SelectCard();

  // Synchronize card with SPI
  for (i = 0; i < 20; i++) {
    SD_HAL_TransmitData(0xff);
  }

  SD_ResponseR1 resp; // response R1 token

  // send CMD0
  resp.responseR1 = SD_SendCommand(SD_GO_IDLE_STATE, 0);

  // Check response errors
  if (resp.responseR1 != 0x01) {
    println("GO_IDLE_STATE error");
  }

  // send CMD8
  resp.responseR1 = SD_SendCommand(SD_SEND_IF_COND,
      SD_IF_COND_VOLT | SD_IF_COND_CHECK); // voltage range and check pattern

  // CMD8 gets more info
  SD_GetResponseR3orR7(buf);

  // Check response errors
  if (resp.responseR1 != 0x01) {
    println("SEND_IF_COND error");
  }

  // Check if card supports given voltage range
  if ((buf[3] != SD_IF_COND_CHECK) || (buf[2] != (SD_IF_COND_VOLT>>8))) {
    println("SEND_IF_COND error");
    for (i=0; i<4; i++) {
      print("%02x ", buf[i]);
    }
    print("\r\n");

  }

  // CMD58
  resp.responseR1 = SD_SendCommand(SD_READ_OCR, 0);

  SD_GetResponseR3orR7(buf);

  // Check response errors
  if (resp.responseR1 != 0x01) {
    println("READ_OCR error");
  }

  // Send OCR to terminal
  print("OCR value: ");
  for (i=0; i<4; i++) {
    print("%02x ", buf[i]);
  }
  print("\r\n");

  // Send ACMD41 until card goes out of IDLE state
  for (i=0; i<10; i++) {

    resp.responseR1 = SD_SendCommand(SD_APP_CMD, 0);
    resp.responseR1 = SD_SendCommand(SD_ACMD_SEND_OP_COND, SD_ACMD41_HCS);
    // Without this delay card wouldn't initialize the first time after
    // power was connected.
    TIMER_Delay(20);
    if (resp.responseR1 == 0x00) { // Card left IDLE state and no errors
      break;
    }

    if (i == 9) {
      println("Failed to initialize SD card");
      while(1);
    }
  }

  // Read Card Capacity Status - SDSC or SDHC?
  resp.responseR1 = SD_SendCommand(SD_READ_OCR, 0);
  SD_GetResponseR3orR7(buf);

  // Check response errors
  if (resp.responseR1 != 0x00) {
    println("SD_READ_OCR error");
  }

  // Send OCR to terminal
  print("OCR value: ");
  for (i=0; i<4; i++) {
    print("%02x ", buf[i]);
  }
  print("\r\n");

  // check capacity
  if (buf[0] & 0x40) {
    println("SDHC card connected");
    isSDHC = 1;
  } else {
    println("SDSC card connected");
    isSDHC = 0;
  }

  SD_HAL_DeselectCard();

}
/**
 *
 * @param buf
 * @param sector
 * @param count
 * @return
 */
uint8_t SD_ReadSectors(uint8_t* buf, uint32_t sector, uint32_t count) {

  SD_ResponseR1 resp;

  // SDSC cards use byte addressing, SDHC use block addressing
  if (!isSDHC) {
    sector *= 512;
  }

  SD_HAL_SelectCard();

  resp.responseR1 = SD_SendCommand(SD_READ_MULTIPLE_BLOCK, sector);

  if (resp.responseR1 != 0x00) {
    println("SD_READ_MULTIPLE_BLOCK error");
  }

  while (count) {
    while (SD_HAL_TransmitData(0xff) != SD_TOKEN_SBR_MBR_SBW); // wait for data token
    SD_HAL_ReadBuffer(buf, 512);
    SD_HAL_TransmitData(0xff);
    SD_HAL_TransmitData(0xff); // two bytes CRC
    count--;
    buf += 512; // move buffer pointer forward
  }

  resp.responseR1 = SD_SendCommand(SD_STOP_TRANSMISSION, 0);

  // R1b response - check busy flag
  while(!SD_HAL_TransmitData(0xff));

  SD_HAL_DeselectCard();

  return 0;
}

uint8_t SD_WriteSectors(uint8_t* buf, uint32_t sector, uint32_t count) {


  sector *= 512;

  SD_HAL_SelectCard();

  uint8_t status;

  do {
    status = SD_SendCommand(SD_WRITE_MULTIPLE_BLOCK, sector);
    TIMER_Delay(5);
  } while (status != 0);

  SD_HAL_TransmitData(0xff);

  while (count) {
    SD_HAL_TransmitData(0xfc); // send data token
    SD_HAL_ReadBuffer(buf, 512);
    SD_HAL_TransmitData(0xff);
    SD_HAL_TransmitData(0xff); // two bytes CRC
    count--;
    buf += 512; // move buffer pointer forward
  }

  SD_HAL_TransmitData(0xfd); // stop transmission
  SD_HAL_TransmitData(0xff);
  while(!SD_HAL_TransmitData(0xff));

  SD_HAL_DeselectCard();

  return 0;
}
/**
 * @brief Sends a command to the SD card.
 *
 * @details This function works for commands which return 1 byte
 * response - R1 response token. These commands are in the majority.
 *
 * @param cmd Command to send
 * @param args Command arguments: 4 bytes as a 32-bit number
 * @return Returns R1 response token
 */
static uint8_t SD_SendCommand(uint8_t cmd, uint32_t args) {

  SD_HAL_TransmitData(0x40 | cmd);
  SD_HAL_TransmitData(args >> 24); // MSB first
  SD_HAL_TransmitData(args >> 16);
  SD_HAL_TransmitData(args >> 8);
  SD_HAL_TransmitData(args);

  // CRC is irrelevant while using SPI interface - only checked for some commands.
  switch (cmd) {
  case SD_GO_IDLE_STATE:
    SD_HAL_TransmitData(0x95);
    break;
  case SD_SEND_IF_COND:
    SD_HAL_TransmitData(0x87);
    break;
  default:
    SD_HAL_TransmitData(0xff);
  }
  // Practice has shown that a valid response token
  // is sent as the second byte by the card.
  // So, we send a dummy byte first.
  SD_HAL_TransmitData(0xff);
  uint8_t ret = SD_HAL_TransmitData(0xff);
  println("Response to cmd %d is %02x", cmd, ret);

  return ret;
}

/**
 * @brief Get R3 or R7 response from card
 *
 * @details R3 response is for READ_OCR command (it is actually five bytes R1
 * + 4 bytes of OCR read by this function). R7 is for SEND_IF_COND command
 * (also R1 + 4 bytes containing voltage information)
 *
 * @param buf Buffer for response
 */
static void SD_GetResponseR3orR7(uint8_t* buf) {

  uint8_t i = 0;
  buf[i++] = SD_HAL_TransmitData(0xff);
  buf[i++] = SD_HAL_TransmitData(0xff);
  buf[i++] = SD_HAL_TransmitData(0xff);
  buf[i++] = SD_HAL_TransmitData(0xff);

}
/**
 * @}
 */
