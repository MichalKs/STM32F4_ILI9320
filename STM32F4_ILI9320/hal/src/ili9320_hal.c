/**
 * @file: 	ili9320_hal.c
 * @brief:	Hardware abstraction layer for ILI9320
 * @date: 	9 gru 2014
 * @author: Michal Ksiezopolski
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

#include <stm32f4xx.h>
#include <ili9320_hal.h>

#define ILI9320_RST_PORT  GPIOB                 ///< GPIO for reset pin
#define ILI9320_RST_PIN   GPIO_Pin_4            ///< Reset pin
#define ILI9320_RST_CLK   RCC_AHB1Periph_GPIOB  ///< Clock for reset pin

#define ILI9320_REG       (*((volatile unsigned short *) 0x60000000)) ///< Address for writing register number
#define ILI9320_DATA      (*((volatile unsigned short *) 0x60020000)) ///< Address for writing data


/**
 * @brief Initialize ILI9320 hardware layer.
 */
void ILI9320_HAL_HardInit(void) {

  GPIO_InitTypeDef GPIO_InitStructure;

  // Enable GPIO clocks
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_AHB1PeriphClockCmd(ILI9320_RST_CLK, ENABLE);

  /*
   * **************************************************
   * Pin mapping:
   *
   * FSMC_D0  - PD14
   * FSMC_D1  - PD15
   * FSMC_D2  - PD0
   * FSMC_D3  - PD1
   * FSMC_D4  - PE7
   * FSMC_D5  - PE8
   * FSMC_D6  - PE9
   * FSMC_D7  - PE10
   * FSMC_D8  - PE11
   * FSMC_D9  - PE12
   * FSMC_D10 - PE13
   * FSMC_D11 - PE14
   * FSMC_D12 - PE15
   * FSMC_D13 - PD8
   * FSMC_D14 - PD9
   * FSMC_D15 - PD10
   * FSMC_NOE - PD4 - RD
   * FSMC_NWE - PD5 - WR
   * FSMC_NE1 - PD7 - CS
   * FSMC_A16 - PD11 - RS
   * RESET  - PB7
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
 * @brief Function for writing data to ILI9320 through FSMC.
 * @param reg Register address.
 * @param data Data to write.
 */
void ILI9320_HAL_WriteReg(uint16_t reg, uint16_t data) {

  ILI9320_REG = reg;
  ILI9320_DATA = data;
}
/**
 * @brief Function for reading a given register.
 * @param reg Register address.
 * @return Contents of register.
 */
uint16_t ILI9320_HAL_ReadReg(uint16_t reg) {

  ILI9320_REG = reg;
  return ILI9320_DATA;
}
/**
 * @brief Turn reset on.
 */
void ILI9320_HAL_ResetOn(void) {

  GPIO_ResetBits(ILI9320_RST_PORT, ILI9320_RST_PIN);
}
/**
 * @brief Turn reset off.
 */
void ILI9320_HAL_ResetOff(void) {
  GPIO_SetBits(ILI9320_RST_PORT, ILI9320_RST_PIN);
}
