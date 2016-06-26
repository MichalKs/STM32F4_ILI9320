/**
 * @file    comm.c
 * @brief   Communication with PC functions.
 * @date    25 sie 2014
 * @author  Michal Ksiezopolski
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

#include <comm.h>
#include <fifo.h>
#include <stdio.h>
#include <uart.h>
#include <string.h>

#ifndef COMM_DEBUG
  #define COMM_DEBUG
#endif

#ifdef COMM_DEBUG
  #define print(str, args...) printf("COMM--> "str"%s",##args,"\r")
  #define println(str, args...) printf("COMM--> "str"%s",##args,"\r\n")
#else
  #define print(str, args...) (void)0
  #define println(str, args...) (void)0
#endif

/**
 * @addtogroup COMM
 * @{
 */

#define COMM_BUF_LEN_TX UART_BUF_LEN_TX    ///< COMM buffer lengths
#define COMM_BUF_LEN_RX 32     ///< COMM buffer lengths
#define COMM_TERMINATOR '\r'   ///< COMM frame terminator character

static uint8_t rxBuffer[COMM_BUF_LEN_RX]; ///< Buffer for received data.
static uint8_t txBuffer[COMM_BUF_LEN_TX]; ///< Buffer for transmitted data.

static FIFO_TypeDef rxFifo; ///< RX FIFO
static FIFO_TypeDef txFifo; ///< TX FIFO

static uint8_t gotFrame;  ///< Nonzero signals a new frame (number of received frames)

int COMM_TxCallback(char* c);
void COMM_RxCallback(char c);

/**
 * @brief Initialize communication terminal interface.
 * @param baud Required baud rate
 */
void COMM_Init(int baud) {

  // pass baud rate
  // callback for received data and callback for transmitted data
  COMM_HAL_Init(baud, COMM_RxCallback, COMM_TxCallback);

  // Initialize RX FIFO for receiving data from PC
  rxFifo.buf = rxBuffer;
  rxFifo.len = COMM_BUF_LEN_RX;
  FIFO_Add(&rxFifo);

  // Initialize TX FIFO for transferring data to PC
  txFifo.buf = txBuffer;
  txFifo.len = COMM_BUF_LEN_TX;
  FIFO_Add(&txFifo);

}
/**
 * @brief Send a char to PC.
 * @details This function can be called in stubs.c _write
 * function in order for printf to work
 *
 * @param c Char to send.
 */
void COMM_Putc(char c) {

  // disable IRQ so it doesn't screw up FIFO count - leads to errors in transmission
  COMM_HAL_IrqDisable();

  FIFO_Push(&txFifo,(uint8_t)c); // Put data in TX buffer

  // enable transmitter if inactive
  if (!COMM_HAL_IsTxActive()) {
    COMM_HAL_TxEnable();
  }

  // enable IRQ again
  COMM_HAL_IrqEnable();
}
/**
 * @brief Send string to PC
 * @param str C-string to send
 */
void COMM_Println(char* str) {
  for (int i = 0; i < strlen(str); i++) {
    COMM_Putc(str[i]);
  }
  COMM_Putc('\r');
  COMM_Putc('\n');
}
/**
 * @brief Get a char from PC
 * @return Received char.
 * @warning Blocking function! Waits until char is received.
 */
char COMM_Getc(void) {

  uint8_t c;

  while (FIFO_IsEmpty(&rxFifo) == 1); // wait until buffer is not empty
  // buffer not empty => char was received

  FIFO_Pop(&rxFifo,&c); // Get data from RX buffer

  return (char)c;
}
/**
 * @brief Get a complete frame from PC(nonblocking)
 * @param buf Buffer for data (data will be null terminated for easier string manipulation)
 * @param len Length not including terminator character
 * @retval 0 Received frame
 * @retval 1 No frame in buffer
 * @retval 2 Frame error
 * TODO Add maximum length checking so as not to overflow
 */
int COMM_GetFrame(uint8_t* buf, uint8_t* len) {

  uint8_t c;
  *len = 0; // zero out length variable

  if (gotFrame) {
    while (1) {

      // no more data and terminator wasn't reached => error
      if (FIFO_IsEmpty(&rxFifo)) {
        *len = 0;
        println("Invalid frame");
        return 2;
      }
      FIFO_Pop(&rxFifo, &c);
      buf[(*len)++] = c;

      // if end of frame
      if (c == COMM_TERMINATOR) {
        (*len)--; // length without terminator character
        buf[*len] = 0; // USART terminator character converted to NULL terminator
        break;
      }

    }
    gotFrame--;
    return 0;

  } else {

    return 1;
  }

}
/**
 * @brief Callback for receiving data from PC.
 * @param c Data sent from lower layer software.
 */
void COMM_RxCallback(char c) {

  uint8_t res = FIFO_Push(&rxFifo, c); // Put data in RX buffer

  // Checking res to ensure no buffer overflow occurred
  if ((c == COMM_TERMINATOR) && (res == 0)) {
    gotFrame++;
  }
}
/**
 * @brief Callback for transmitting data to lower layer
 * @param c Transmitted data
 * @retval 0 There is no more data in buffer (stop transmitting)
 * @retval 1 Valid data in c
 */
int COMM_TxCallback(char* buf) {

  uint8_t* localBuff = (uint8_t*)buf;

  if (FIFO_IsEmpty(&txFifo)) {
    return 0;
  }

  // get all the data at one go
  int i = 0;
  while (!FIFO_Pop(&txFifo, localBuff+i)) {
    i++;
  }

  return i;
}

/**
 * @}
 */
