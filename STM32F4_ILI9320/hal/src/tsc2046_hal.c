/**
 * @file    tsc2046_hal.c
 * @brief	  
 * @date    16 gru 2014
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

#include <tsc2046_hal.h>
#include <stm32f4xx.h>

static void (*penirqCallback)(void); ///< PENIRQ interrupt callback function

/**
 * @brief Initialize PENIRQ signal and interrupt.
 * @param penirqCb Callback function for PENIRQ interrupt
 */
void TSC2046_HAL_PenirqInit(void (*penirqCb)(void)) {

  penirqCallback = penirqCb;

  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the PENIRQ Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure PENIRQ pin as input */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);


  /* Connect Button EXTI Line to PENIRQ GPIO Pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource2);

  /* Configure PENIRQ EXTI line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set PENIRQ EXTI Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

uint8_t TSC2046_HAL_ReadPenirq(void) {
  return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2);
}

/**
 * @brief Clear PENIRQ flag and enable PENIRQ interrupt.
 */
void TSC2046_HAL_EnablePenirq(void) {
  EXTI_ClearITPendingBit(EXTI_Line2);
  NVIC_EnableIRQ(EXTI2_IRQn);
}
/**
 * @brief Disable PENIRQ interrupt.
 */
void TSC2046_HAL_DisablePenirq(void) {
  NVIC_DisableIRQ(EXTI2_IRQn);
}
/**
 * @brief Handler for PENIRQ interrupt.
 */
void EXTI2_IRQHandler(void) {

  if (EXTI_GetITStatus(EXTI_Line2) != RESET) {

    penirqCallback();
    EXTI_ClearITPendingBit(EXTI_Line2);

  }

}
