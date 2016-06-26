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
#include <stdio.h>
#include "common_hal.h"

#define ILI9320_RST_PORT  GPIOB                 ///< GPIO for reset pin
#define ILI9320_RST_PIN   GPIO_PIN_4            ///< Reset pin
#define ILI9320_RST_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()  ///< Clock for reset pin

#define ILI9320_REG       (*((volatile unsigned short *) 0x60000000)) ///< Address for writing register number
#define ILI9320_DATA      (*((volatile unsigned short *) 0x60020000)) ///< Address for writing data

SRAM_HandleTypeDef hsram;
FSMC_NORSRAM_TimingTypeDef timing;

/**
 * @brief Initialize ILI9320 hardware layer.
 */
void ILI9320_HAL_HardInit(void) {

  GPIO_InitTypeDef GPIO_InitStructure;

  // Enable GPIO clocks
  ILI9320_RST_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*
   * **************************************************
   * PIN mapping:
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
   * RESET  - PB4
   * **************************************************
   */

  // FSMC pins as AF

  GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 |
      GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
      GPIO_PIN_11 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStructure.Mode   = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Speed  = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Pull   = GPIO_NOPULL;
  GPIO_InitStructure.Alternate = GPIO_AF12_FSMC;

  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |
  GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
  GPIO_PIN_14 | GPIO_PIN_15;

  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

  // Reset pin as out push-pull
  GPIO_InitStructure.Pin = ILI9320_RST_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ILI9320_RST_PORT, &GPIO_InitStructure);

  // Enable FSMC
  __HAL_RCC_FSMC_CLK_ENABLE();
//  printf("%s 0x%08x\r\n", __FUNCTION__, (unsigned int)FSMC_Bank1->BTCR[0]);
//  FSMC_Bank1->BTCR[0] = FSMC_BCR1_MBKEN | FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN;
//  printf("%s 0x%08x\r\n", __FUNCTION__, (unsigned int)FSMC_Bank1->BTCR[0]);
//  FSMC_Bank1->BTCR[1] = 0x1404;

  hsram.Instance  = FSMC_NORSRAM_DEVICE;
  hsram.Extended  = FSMC_NORSRAM_EXTENDED_DEVICE;

  timing.AddressSetupTime       = 4;
  timing.AddressHoldTime        = 0;
  timing.DataSetupTime          = 0x14;
  timing.BusTurnAroundDuration  = 0;
  timing.CLKDivision            = 0;
  timing.DataLatency            = 0;
  timing.AccessMode             = FSMC_ACCESS_MODE_B;

  hsram.Init.NSBank             = FSMC_NORSRAM_BANK1;
  hsram.Init.DataAddressMux     = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram.Init.MemoryType         = FSMC_MEMORY_TYPE_SRAM;
  hsram.Init.MemoryDataWidth    = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram.Init.BurstAccessMode    = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram.Init.WrapMode           = FSMC_WRAP_MODE_DISABLE;
  hsram.Init.WaitSignalActive   = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram.Init.WriteOperation     = FSMC_WRITE_OPERATION_ENABLE;
  hsram.Init.WaitSignal         = FSMC_WAIT_SIGNAL_DISABLE;
  hsram.Init.ExtendedMode       = FSMC_EXTENDED_MODE_DISABLE;
  hsram.Init.AsynchronousWait   = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram.Init.WriteBurst         = FSMC_WRITE_BURST_DISABLE;

  /* Initialize the SRAM controller */
  if(HAL_SRAM_Init(&hsram, &timing, &timing) != HAL_OK) {
    /* Initialization Error */
    COMMON_HAL_ErrorHandler();
  }


//  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAM_InitStructure;
//  FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAM_Timing;
//
//  FSMC_NORSRAM_Timing.FSMC_AddressSetupTime       = 0x04;
//  FSMC_NORSRAM_Timing.FSMC_AddressHoldTime        = 0x00;
//  FSMC_NORSRAM_Timing.FSMC_DataSetupTime          = 0x14;
//  FSMC_NORSRAM_Timing.FSMC_BusTurnAroundDuration  = 0x00;
//  FSMC_NORSRAM_Timing.FSMC_CLKDivision            = 0x00;
//  FSMC_NORSRAM_Timing.FSMC_DataLatency            = 0x00;
//  FSMC_NORSRAM_Timing.FSMC_AccessMode             = FSMC_AccessMode_B;

//  FSMC_NORSRAM_InitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAM1;
//  FSMC_NORSRAM_InitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
//  FSMC_NORSRAM_InitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR;
//  FSMC_NORSRAM_InitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
//  FSMC_NORSRAM_InitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
//  FSMC_NORSRAM_InitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
//  FSMC_NORSRAM_InitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;
//  FSMC_NORSRAM_InitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;
//  FSMC_NORSRAM_InitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;
//  FSMC_NORSRAM_InitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
//  FSMC_NORSRAM_InitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable;
//  FSMC_NORSRAM_InitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;
//  FSMC_NORSRAM_InitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAM_Timing;
//  FSMC_NORSRAM_InitStructure.FSMC_WriteTimingStruct     = &FSMC_NORSRAM_Timing;

//  FSMC_NORSRAMInit(&FSMC_NORSRAM_InitStructure);

  /* Enable FSMC Bank1_SRAM Bank */
//  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);

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

  HAL_GPIO_WritePin(ILI9320_RST_PORT, ILI9320_RST_PIN, GPIO_PIN_RESET);
}
/**
 * @brief Turn reset off.
 */
void ILI9320_HAL_ResetOff(void) {
  HAL_GPIO_WritePin(ILI9320_RST_PORT, ILI9320_RST_PIN, GPIO_PIN_SET);
}
