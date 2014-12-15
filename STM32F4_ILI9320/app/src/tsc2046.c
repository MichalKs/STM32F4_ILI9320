/**
 * @file    tsc2046.c
 * @brief	  
 * @date    14 gru 2014
 * @author  Michal Ksiezopolski
 *
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


#include <tsc2046.h>
#include <spi1.h>
#include <utils.h>
#include <stdio.h>
#include <stm32f4xx.h>

#define DEBUG

#ifdef DEBUG
#define print(str, args...) printf(""str"%s",##args,"")
#define println(str, args...) printf("TSC--> "str"%s",##args,"\r\n")
#else
#define print(str, args...) (void)0
#define println(str, args...) (void)0
#endif


/*
 * Settings of the analog input pins
 */
#define MEASURE_TEMP0   0b000
#define MEASURE_Y       0b001
#define MEASURE_VBAT    0b010
#define MEASURE_Z1      0b011
#define MEASURE_Z2      0b100
#define MEASURE_X       0b101
#define MEASURE_AUX     0b110
#define MEASURE_TEMP1   0b111


/**
 * @brief Control byte
 *
 * @details MSB is the start bit (always 1). Then come 3
 * analog input bits, mode bit (12bit = 0, 8bit = 1).
 * Next comes the SER/DFR bit (single ended mode = 1,
 * differential mode = 0). Two LSB bits are the
 * power down mode select bits.
 */
typedef union {
  struct {
    uint8_t powerDown :2;
    uint8_t channelSelect :3;
    uint8_t mode: 1;
    uint8_t serDfr :1;
    uint8_t startBit :1;
  } bits;
  uint8_t byte;
} ControlByteTypedef;

/**
 * @brief Initialize the touchscreen library.
 */
void TSC2046_Init(void) {

  SPI1_Init();

}
/**
 * @brief Read position
 * FIXME This is just a test function for now.
 * TODO Place low level stuff in HAL
 */
void TSC2046_ReadPos(void) {

  NVIC_DisableIRQ(EXTI1_IRQn);

  SPI1_Select();
  uint8_t buf[3];
  uint8_t* ptr = buf;

  *ptr++ = SPI1_Transmit(0b10010000);
  *ptr++ = SPI1_Transmit(0xff);
  *ptr++ = SPI1_Transmit(0xff);

  uint16_t result = buf[2]|((uint16_t)buf[1]<<8);

  result>>=3;

  println("Data from TSC: %u", result);
//  hexdump(buf, 3);

  SPI1_Deselect();
  EXTI_ClearITPendingBit(EXTI_Line1);
  NVIC_EnableIRQ(EXTI1_IRQn);

}
