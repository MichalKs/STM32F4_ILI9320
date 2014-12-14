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

#define DEBUG

#ifdef DEBUG
#define print(str, args...) printf(""str"%s",##args,"")
#define println(str, args...) printf("TSC--> "str"%s",##args,"\r\n")
#else
#define print(str, args...) (void)0
#define println(str, args...) (void)0
#endif

/**
 * @brief Control byte
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

  SPI1_Select();
  uint8_t buf[20];
  uint8_t* ptr = buf;
  *ptr++ = SPI1_Transmit(0b10010011);
  *ptr++ = SPI1_Transmit(0xff);
  *ptr++ = SPI1_Transmit(0xff);

  *ptr++ = SPI1_Transmit(0b11010011);
  *ptr++ = SPI1_Transmit(0xff);
  *ptr++ = SPI1_Transmit(0xff);

  println("Data from TSC:");
  hexdump(buf, 6);
  SPI1_Deselect();

}
