/**
 * @file    keys_hal.c
 * @brief   Matrix keyboard low level functions
 * @date    6 paź 2014
 * @author  Michal Ksiezopolski
 * 
 * FIXME This module in not finished nor tested with HAL
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

#include <keys_hal.h>
#include <stm32f4xx_hal.h>

/**
 * @addtogroup KEYS_HAL
 * @{
 */
/*
 * Pin and port mappings for matrix keyboard.
 */
#define KEYS_ROW0_PIN GPIO_PIN_11
#define KEYS_ROW1_PIN GPIO_PIN_12
#define KEYS_ROW2_PIN GPIO_PIN_13
#define KEYS_ROW3_PIN GPIO_PIN_14
#define KEYS_COL0_PIN GPIO_PIN_7
#define KEYS_COL1_PIN GPIO_PIN_8
#define KEYS_COL2_PIN GPIO_PIN_9
#define KEYS_COL3_PIN GPIO_PIN_10

#define KEYS_ROW_PORT   GPIOE
#define KEYS_ROW_CLOCK_ENABLE()  __HAL_RCC_GPIOE_CLK_ENABLE()

#define KEYS_COL_PORT   GPIOE
#define KEYS_COL_CLOCK_ENABLE()  __HAL_RCC_GPIOE_CLK_ENABLE()

/**
 * @brief Initialize 4x4 matrix keyboard
 */
void KEYS_HAL_Init(void) {

  // Enable clocks
  KEYS_ROW_CLOCK_ENABLE();
  KEYS_COL_CLOCK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  GPIO_InitStruct.Pin = KEYS_ROW0_PIN | KEYS_ROW1_PIN |
      KEYS_ROW2_PIN | KEYS_ROW3_PIN;

  HAL_GPIO_Init(KEYS_ROW_PORT, &GPIO_InitStruct);

  // Configure column pins in output push/pull mode
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  GPIO_InitStruct.Pin = KEYS_COL0_PIN | KEYS_COL1_PIN |
      KEYS_COL2_PIN | KEYS_COL3_PIN;

  HAL_GPIO_Init(KEYS_COL_PORT, &GPIO_InitStruct);

}
/**
 * @brief Select a column
 * @param col Column number
 */
void KEYS_HAL_SelectColumn(uint8_t col) {

  // set all columns high
  HAL_GPIO_WritePin(KEYS_COL_PORT, KEYS_COL0_PIN | KEYS_COL1_PIN |
      KEYS_COL2_PIN | KEYS_COL3_PIN, GPIO_PIN_SET);

  // set selected column as low
  switch (col) {
  case 0:
    HAL_GPIO_WritePin(KEYS_COL_PORT, KEYS_COL0_PIN, GPIO_PIN_RESET);
    break;
  case 1:
    HAL_GPIO_WritePin(KEYS_COL_PORT, KEYS_COL1_PIN, GPIO_PIN_RESET);
    break;
  case 2:
    HAL_GPIO_WritePin(KEYS_COL_PORT, KEYS_COL2_PIN, GPIO_PIN_RESET);
    break;
  case 3:
    HAL_GPIO_WritePin(KEYS_COL_PORT, KEYS_COL3_PIN, GPIO_PIN_RESET);
    break;
  default:
    break;
  }

}
/**
 * @brief Read keyboard row.
 * @return Row value
 */
int8_t KEYS_HAL_ReadRow(void) {

  uint16_t row_p = (GPIO_ReadInputData(KEYS_ROW_PORT)); // read row
  uint16_t row = ~row_p; // negate, because we use low level for keypress

  if (row & KEYS_ROW0_PIN)
    return 0;
  if (row & KEYS_ROW1_PIN)
    return 1;
  if (row & KEYS_ROW2_PIN)
    return 2;
  if (row & KEYS_ROW3_PIN)
    return 3;

  return -1;
}
/**
 * @}
 */
