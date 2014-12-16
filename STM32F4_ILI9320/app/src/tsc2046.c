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
#include <tsc2046_hal.h>
#include <utils.h>
#include <stdio.h>
#include <led.h>
#include <timers.h>

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

#define MODE_8BIT   1
#define MODE_12BIT  0

#define SINGLE_ENDED 1
#define DIFFERENTIAL 0

#define PD_POWER_DOWN 0b00
#define PD_VREF_OFF   0b01
#define PD_ADC_OFF    0b10
#define PD_ALWAYS_ON  0b11

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
    uint8_t serDfr :1;
    uint8_t mode: 1;
    uint8_t channelSelect :3;
    uint8_t startBit :1;
  } bits;
  uint8_t byte;
} ControlByteTypedef;

static uint8_t penirqAsserted; ///< Is PENIRQ low?

static void penirqCallback(void);
void TSC2046_ReadPos(uint16_t *x, uint16_t *y);

/**
 * @brief Initialize the touchscreen library.
 */
void TSC2046_Init(void) {

  // initialize SPI interface
  SPI1_Init();
  // initialize PENIRQ signal handling
  TSC2046_HAL_PenirqInit(penirqCallback);

  // send first commands
  // with 2 LSB bits = 0 to enable PENIRQ

  ControlByteTypedef ctrl;
  ctrl.bits.startBit = 1;
  ctrl.bits.serDfr = DIFFERENTIAL;
  ctrl.bits.mode = MODE_12BIT;
  ctrl.bits.powerDown = PD_POWER_DOWN;
  ctrl.bits.channelSelect = MEASURE_X;

  SPI1_Select();
  SPI1_Transmit(ctrl.byte);
  SPI1_Transmit(0);
  SPI1_Transmit(0);
  SPI1_Deselect();
}
/**
 * @brief Handler for touchscreen actions.
 *
 * @details Call this function regularly in main to handle
 * touchscreen events.
 *
 * FIXME Just test for now
 */
void TSC2046_Update(void) {

  static uint32_t counter = 0;
  static uint8_t irqReceived = 0;
  uint16_t x, y;

  if (irqReceived == 1) { // init counter
    counter = TIMER_GetTime();
    irqReceived = 2;
  } else if (irqReceived == 2) { // debounce delay
    if (TIMER_GetTime()- counter >= 20) {
      irqReceived = 3;
      if (!TSC2046_HAL_ReadPenirq()) {
        TSC2046_ReadPos(&x, &y);

        // Toggle LED for given region
        if (y <= 2000) {
          if (x <= 2000) {
            LED_Toggle(LED0);
          } else {
            LED_Toggle(LED1);
          }
        } else {
          if (x <= 2000) {
            LED_Toggle(LED1);
          } else {
            LED_Toggle(LED0);
          }
        }
      }
    }
  } else if (irqReceived == 3) { // wait for next measurement
    if (TIMER_GetTime()- counter >= 100) {
      irqReceived = 0;
      penirqAsserted = 0;
    }
  } else if (penirqAsserted) { // irq received
    irqReceived = 1;
  }

}
/**
 * @brief Read X and Y position on touchscreen.
 * @param x Pointer to store X coordinate.
 * @param y Pointer to store Y coordinate.
 */
void TSC2046_ReadPos(uint16_t *x, uint16_t *y) {

  TSC2046_HAL_DisablePenirq(); // disable IRQ during read
  SPI1_Select();

  // control byte
  ControlByteTypedef ctrl;
  ctrl.bits.startBit = 1;
  ctrl.bits.serDfr = DIFFERENTIAL;
  ctrl.bits.mode = MODE_12BIT;
  ctrl.bits.powerDown = PD_POWER_DOWN;
  ctrl.bits.channelSelect = MEASURE_Y;

  uint16_t tmpX, tmpY;

  // read Y
  SPI1_Transmit(ctrl.byte);
  tmpY = ((uint16_t)SPI1_Transmit(0))<<8;
  tmpY |= SPI1_Transmit(0);

  ctrl.bits.channelSelect = MEASURE_X;

  // read X
  SPI1_Transmit(ctrl.byte);
  tmpX = ((uint16_t)SPI1_Transmit(0))<<8;
  tmpX |= SPI1_Transmit(0);

  *y = tmpY >> 3;
  *x = tmpX >> 3;

  println("Data from TSC: x=%u y=%u", *x, *y);

  SPI1_Deselect();
  TSC2046_HAL_EnablePenirq(); // reenable IRQ

}

/**
 * @brief Callback function called by lower layer whenever
 * PENIRQ signal is asserted. This signalized that the
 * touchscreen was pressed.
 */
static void penirqCallback(void) {
  penirqAsserted = 1;
}
