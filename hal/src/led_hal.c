/**
 * @file    led_hal.c
 * @brief   HAL for using LEDs
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

#include "led_hal.h"
#include <stm32f4xx_hal.h>

/**
 * @addtogroup LED_HAL
 * @{
 */

/**
 * @brief LED GPIO ports
 */
static GPIO_TypeDef* ledPort[MAX_LEDS] = {
    GPIOD,
    GPIOD,
    GPIOD,
    GPIOD};
/**
 * @brief LED pin numbers
 */
static uint32_t ledPin[MAX_LEDS] = {
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15};

/**
 * @brief Add an LED.
 * @param led LED number.
 */
void LED_HAL_Init(uint8_t led) {

  if (ledPort[led] == GPIOA) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
  } else if (ledPort[led] == GPIOB) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
  } else if (ledPort[led] == GPIOC) {
    __HAL_RCC_GPIOC_CLK_ENABLE();
  } else if (ledPort[led] == GPIOD) {
    __HAL_RCC_GPIOD_CLK_ENABLE();
  } else if (ledPort[led] == GPIOE) {
    __HAL_RCC_GPIOE_CLK_ENABLE();
  } else if (ledPort[led] == GPIOF) {
    __HAL_RCC_GPIOF_CLK_ENABLE();
  } else if (ledPort[led] == GPIOG) {
    __HAL_RCC_GPIOG_CLK_ENABLE();
  }

  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  GPIO_InitStruct.Pin = ledPin[led];

  HAL_GPIO_Init(ledPort[led], &GPIO_InitStruct);

  HAL_GPIO_WritePin(ledPort[led], ledPin[led], GPIO_PIN_RESET); // turn LED off

}

/**
 * @brief Toggle an LED.
 * @param led LED number.
 */
void LED_HAL_Toggle(uint8_t led) {

  ledPort[led]->ODR ^= ledPin[led]; // toggle bit
}

/**
 * @brief Change the state of an LED.
 * @param led LED number.
 * @param state New state.
 */
void LED_HAL_ChangeState(uint8_t led, uint8_t state) {

  if (state == 1) {
    ledPort[led]->BSRR = ledPin[led]; // set bit
  } else {
    ledPort[led]->BSRR = (ledPin[led]<<16); // reset bit
  }

}

/**
 * @}
 */
