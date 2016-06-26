/**
 * @file    common_hal.c
 * @brief
 * @date    25 cze 2016
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

#include "common_hal.h"
#include <stm32f4xx_hal.h>
#include <led_hal.h>

#define HAL_ERROR_LED_NUMBER 3 ///< Number of LED for HAL errors

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  */
static void systemClockConfig(void) {
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState        = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM        = 8;
  RCC_OscInitStruct.PLL.PLLN        = 336;
  RCC_OscInitStruct.PLL.PLLP        = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ        = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    COMMON_HAL_ErrorHandler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType       = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider  = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    COMMON_HAL_ErrorHandler();
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001) {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/**
 * @brief Initialize HAL layer
 */
void COMMON_HAL_Init(void) {

  // initialize LED for signaling errors
  LED_HAL_Init(HAL_ERROR_LED_NUMBER); // Add an LED
  LED_HAL_ChangeState(HAL_ERROR_LED_NUMBER, 0);

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();

  // Configure the system clock to 168 MHz
  systemClockConfig();
}

/**
 * @brief Error handler for HAL
 */
void COMMON_HAL_ErrorHandler(void) {
  LED_HAL_ChangeState(HAL_ERROR_LED_NUMBER, 1);
  while(1) {

  }
}


/**
  * @brief   This function handles NMI exception.
  */
void NMI_Handler(void) {

}

/**
  * @brief  This function handles Hard Fault exception.
  */
void HardFault_Handler(void) {
  LED_HAL_ChangeState(HAL_ERROR_LED_NUMBER, 1);
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1) {

  }
}

/**
  * @brief  This function handles Memory Manage exception.
  */
void MemManage_Handler(void) {
  LED_HAL_ChangeState(HAL_ERROR_LED_NUMBER, 1);
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1) {

  }
}

/**
  * @brief  This function handles Bus Fault exception.
  */
void BusFault_Handler(void) {
  LED_HAL_ChangeState(HAL_ERROR_LED_NUMBER, 1);
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1) {

  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void) {
  LED_HAL_ChangeState(HAL_ERROR_LED_NUMBER, 1);
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1) {

  }
}

/**
  * @brief  This function handles SVCall exception.
  */
void SVC_Handler(void) {

}

/**
  * @brief  This function handles Debug Monitor exception.
  */
void DebugMon_Handler(void) {

}

/**
  * @brief  This function handles PendSVC exception.
  */
void PendSV_Handler(void) {

}

