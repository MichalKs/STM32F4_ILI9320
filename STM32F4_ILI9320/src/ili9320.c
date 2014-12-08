/**
 * @file: 	ili9320.c
 * @brief:	   
 * @date: 	22 maj 2014
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

#include "ili9320.h"
#include "timers.h"
#include <stdio.h>

/*
 * ILI9320 driver commands/registers
 */
#define ILI9320_START_OSCILLATION 0x00
#define ILI9320_READ_ID           0x00
#define ILI9320_DRIVER_OUTPUT     0x01
#define ILI9320_DRIVING_WAVE      0x02
#define ILI9320_ENTRY_MODE        0x03
#define ILI9320_RESIZE            0x04
#define ILI9320_DISP1             0x07
#define ILI9320_DISP2             0x08
#define ILI9320_DISP3             0x09
#define ILI9320_DISP4             0x0a
#define ILI9320_RGB_DISP1         0x0c
#define ILI9320_FRAME_MARKER      0x0d
#define ILI9320_RGB_DISP2         0x0f
#define ILI9320_POWER1            0x10
#define ILI9320_POWER2            0x11
#define ILI9320_POWER3            0x12
#define ILI9320_POWER4            0x13
#define ILI9320_HOR_GRAM_ADDR     0x20
#define ILI9320_VER_GRAM_ADDR     0x21
#define ILI9320_WRITE_TO_GRAM     0x22
#define ILI9320_POWER7            0x29
#define ILI9320_FRAME_RATE        0x2b
#define ILI9320_GAMMA1            0x30
#define ILI9320_GAMMA2            0x31
#define ILI9320_GAMMA3            0x32
#define ILI9320_GAMMA4            0x35
#define ILI9320_GAMMA5            0x36
#define ILI9320_GAMMA6            0x37
#define ILI9320_GAMMA7            0x38
#define ILI9320_GAMMA8            0x39
#define ILI9320_GAMMA9            0x3c
#define ILI9320_GAMMA10           0x3d
#define ILI9320_HOR_ADDR_START    0x50
#define ILI9320_HOR_ADDR_END      0x51
#define ILI9320_VER_ADDR_START    0x52
#define ILI9320_VER_ADDR_END      0x53
#define ILI9320_DRIVER_OUTPUT2    0x60
#define ILI9320_BASE_IMAGE        0x61
#define ILI9320_VERTICAL_SCROLL   0x6a
#define ILI9320_PARTIAL1_POS      0x80
#define ILI9320_PARTIAL1_START    0x81
#define ILI9320_PARTIAL1_END      0x82
#define ILI9320_PARTIAL2_POS      0x83
#define ILI9320_PARTIAL2_START    0x84
#define ILI9320_PARTIAL2_END      0x85
#define ILI9320_PANEL_INTERFACE1  0x90
#define ILI9320_PANEL_INTERFACE2  0x92
#define ILI9320_PANEL_INTERFACE3  0x93
#define ILI9320_PANEL_INTERFACE4  0x95
#define ILI9320_PANEL_INTERFACE5  0x97
#define ILI9320_PANEL_INTERFACE6  0x98

#define ILI9320_RST_PORT  GPIOB
#define ILI9320_RST_PIN   GPIO_Pin_4
#define ILI9320_RST_CLK   RCC_AHB1Periph_GPIOB

#define ILI9320_REG       (*((volatile unsigned short *) 0x60000000))
#define ILI9320_DATA      (*((volatile unsigned short *) 0x60020000))

static void ILI9320_HardInit(void);
static void ILI9320_WriteReg(uint16_t reg, uint16_t data);
static uint16_t ILI9320_ReadReg(uint16_t reg);
uint16_t ILI9320_RGBDecode(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Initialize the ILI9320 TFT LCD driver.
 */
void ILI9320_Initializtion(void) {

  ILI9320_HardInit(); // GPIO and FSMC init

  // Reset the LCD
  GPIO_SetBits(ILI9320_RST_PORT, ILI9320_RST_PIN);
  TIMER_Delay(500);
  GPIO_ResetBits(ILI9320_RST_PORT, ILI9320_RST_PIN);

  TIMER_Delay(500);
  GPIO_SetBits(ILI9320_RST_PORT, ILI9320_RST_PIN);
  TIMER_Delay(500);

  ILI9320_WriteReg(ILI9320_START_OSCILLATION, 0x0001);
  TIMER_Delay(200);

  // Read LCD ID
  unsigned int id;
  id = ILI9320_ReadReg(ILI9320_READ_ID);

  printf("ID TFT LCD = %x\r\n", id);

  // Add more LCD init codes here
  if (id == 0x9320) {

    ILI9320_WriteReg(ILI9320_DRIVER_OUTPUT, 0x0100); // SS = 1 - coordinates from left to right
    ILI9320_WriteReg(ILI9320_DRIVING_WAVE, 0x0700);  // Line inversion
    ILI9320_WriteReg(ILI9320_ENTRY_MODE, 0x1018);    //
    ILI9320_WriteReg(ILI9320_RESIZE, 0x0000);
    ILI9320_WriteReg(ILI9320_DISP1, 0x0000);
    ILI9320_WriteReg(ILI9320_DISP2, 0x0202); // two lines back porch, two line front porch
    ILI9320_WriteReg(ILI9320_DISP3, 0x0000);
    ILI9320_WriteReg(ILI9320_DISP4, 0x0000);
    ILI9320_WriteReg(ILI9320_RGB_DISP1, 0x0001);
    ILI9320_WriteReg(ILI9320_FRAME_MARKER, 0x0000); // 0th line for frame marker
    ILI9320_WriteReg(ILI9320_RGB_DISP2, 0x0000);
    ILI9320_WriteReg(ILI9320_DISP1, 0x0101);
    ILI9320_WriteReg(ILI9320_POWER1, 0x10c0);
    ILI9320_WriteReg(ILI9320_POWER2, 0x0007);
    ILI9320_WriteReg(ILI9320_POWER3, 0x0110);
    ILI9320_WriteReg(ILI9320_POWER4, 0x0b00);
    ILI9320_WriteReg(ILI9320_POWER7, 0x0000);
    ILI9320_WriteReg(ILI9320_FRAME_RATE, 0x4010);

    // Set window
    ILI9320_WriteReg(ILI9320_HOR_ADDR_START, 0);
    ILI9320_WriteReg(ILI9320_HOR_ADDR_END, 239);
    ILI9320_WriteReg(ILI9320_VER_ADDR_START, 0);
    ILI9320_WriteReg(ILI9320_VER_ADDR_END, 319);

    ILI9320_WriteReg(ILI9320_DRIVER_OUTPUT2, 0x2700);
    ILI9320_WriteReg(ILI9320_BASE_IMAGE, 0x0001);
    ILI9320_WriteReg(ILI9320_VERTICAL_SCROLL, 0x0000);
    ILI9320_WriteReg(ILI9320_PARTIAL1_POS, 0x0000);
    ILI9320_WriteReg(ILI9320_PARTIAL1_START, 0x0000);
    ILI9320_WriteReg(ILI9320_PARTIAL1_END, 0x0000);
    ILI9320_WriteReg(ILI9320_PARTIAL2_POS, 0x0000);
    ILI9320_WriteReg(ILI9320_PARTIAL2_START, 0x0000);
    ILI9320_WriteReg(ILI9320_PARTIAL2_END, 0x0000);
    ILI9320_WriteReg(ILI9320_PANEL_INTERFACE1, 0x0010);
    ILI9320_WriteReg(ILI9320_PANEL_INTERFACE2, 0x0000);
    ILI9320_WriteReg(ILI9320_PANEL_INTERFACE3, 0x0001);
    ILI9320_WriteReg(ILI9320_PANEL_INTERFACE4, 0x0110);
    ILI9320_WriteReg(ILI9320_PANEL_INTERFACE5, 0x0000);
    ILI9320_WriteReg(ILI9320_PANEL_INTERFACE6, 0x0000);
    ILI9320_WriteReg(ILI9320_DISP1, 0x0173);

  }

  TIMER_Delay(100);
}

/**
 * @brief Initialize GPIO
 */
static void ILI9320_HardInit(void) {

  GPIO_InitTypeDef GPIO_InitStructure;

  // Enable GPIO clocks
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_AHB1PeriphClockCmd(ILI9320_RST_CLK, ENABLE);

  /*
   * **************************************************
   * Pin mapping:
   *
   * FSMC_D0  -	PD14
   * FSMC_D1	-	PD15
   * FSMC_D2	-	PD0
   * FSMC_D3	-	PD1
   * FSMC_D4	-	PE7
   * FSMC_D5	-	PE8
   * FSMC_D6	-	PE9
   * FSMC_D7	-	PE10
   * FSMC_D8	-	PE11
   * FSMC_D9	-	PE12
   * FSMC_D10	-	PE13
   * FSMC_D11	-	PE14
   * FSMC_D12	-	PE15
   * FSMC_D13 -	PD8
   * FSMC_D14 -	PD9
   * FSMC_D15	-	PD10
   * FSMC_NOE	-	PD4 - RD
   * FSMC_NWE	-	PD5 - WR
   * FSMC_NE1 -	PD7 - CS
   * FSMC_A16	-	PD11 - RS
   * RESET	-	PB7
   * **************************************************
   */

  // FSMC pins as AF
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 |
  GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
  GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
  GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
  GPIO_Pin_14 | GPIO_Pin_15;

  GPIO_Init(GPIOE, &GPIO_InitStructure);

  // Reset pin as out push-pull
  GPIO_InitStructure.GPIO_Pin = ILI9320_RST_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(ILI9320_RST_PORT, &GPIO_InitStructure);

  // Enable FSMC
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
  FSMC_Bank1->BTCR[0] = FSMC_BCR2_MBKEN | FSMC_BCR2_MWID_0 | FSMC_BCR1_WREN;
  FSMC_Bank1->BTCR[1] = 0x1404;
}

/**
 *
 * @param r
 * @param g
 * @param b
 * @return
 */
uint16_t ILI9320_RGBDecode(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0x1f) << 11) | ((g & 0x3f) << 5) | (b & 0x1f);
}

/**
 * @brief Move cursor to given coordinates.
 * @param x X coordinate
 * @param y Y coordinate
 */
void ILI9320_SetCursor(uint16_t x, uint16_t y) {

  ILI9320_WriteReg(ILI9320_HOR_GRAM_ADDR, y);
  ILI9320_WriteReg(ILI9320_VER_GRAM_ADDR, x);

}
/**
 *
 * @param x
 * @param y
 * @param color
 */
void ILI9320_DrawPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {

  ILI9320_SetCursor(x, y);
  ILI9320_WriteReg(ILI9320_WRITE_TO_GRAM, ILI9320_RGBDecode(r, g, b));
}
/**
 *
 * @param x
 * @param y
 * @param width
 * @param height
 */
void ILI9320_SetWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {

  ILI9320_SetCursor(x, y);
  ILI9320_WriteReg(ILI9320_HOR_ADDR_START, y);
  ILI9320_WriteReg(ILI9320_HOR_ADDR_END, y + height - 1);
  ILI9320_WriteReg(ILI9320_VER_ADDR_START, x);
  ILI9320_WriteReg(ILI9320_VER_ADDR_END, x + width - 1);
}

/**
 *
 * @param reg
 * @param data
 */
static void ILI9320_WriteReg(uint16_t reg, uint16_t data) {

  ILI9320_REG = reg;
  ILI9320_DATA = data;
}
/**
 *
 * @param reg
 * @return
 */
static uint16_t ILI9320_ReadReg(uint16_t reg) {

  ILI9320_REG = reg;
  return ILI9320_DATA;
}

