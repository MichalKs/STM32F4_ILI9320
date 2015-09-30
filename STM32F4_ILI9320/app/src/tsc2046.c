/**
 * @file    tsc2046.c
 * @brief   TSC2046 touchscreen library
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
#include <spi3.h>
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

/**
 * @addtogroup TSC2046
 * @{
 */

/*
 * Settings of the analog input pins
 */
#define MEASURE_TEMP0   0b000 ///< Measure temperature
#define MEASURE_Y       0b001 ///< Measure Y position
#define MEASURE_VBAT    0b010 ///< Measure battery voltage
#define MEASURE_Z1      0b011
#define MEASURE_Z2      0b100
#define MEASURE_X       0b101 ///< Measure X position
#define MEASURE_AUX     0b110
#define MEASURE_TEMP1   0b111 ///< Measure temperature

#define MODE_8BIT   1 ///< 8-bit data mode
#define MODE_12BIT  0 ///< 12-bit data mode

#define SINGLE_ENDED 1 ///< Single ended measurement
#define DIFFERENTIAL 0 ///< Differential measurement

#define PD_POWER_DOWN 0b00 ///< Power down mode
#define PD_VREF_OFF   0b01 ///< Turn off Vref
#define PD_ADC_OFF    0b10 ///< Turn off ADC
#define PD_ALWAYS_ON  0b11 ///< No power down

#define DEBOUNCE_TIME 20 ///< Debounce time for touchsreen
#define WAIT_TIME 100 ///<

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

#define MAX_EVENTS 20 ///< Maximum number of registered events
/**
 * @brief Structure for defining an event triggered but touching
 * a specific region of the touchscreen.
 *
 * TODO Implement hold and swipes.
 */
typedef struct {
  void (*cb)(uint16_t x, uint16_t y); ///< Callback function for event (gets exact coordinates of touch)
  uint16_t x;       ///< X coordinate of event region origin
  uint16_t y;       ///< Y coordinate of event region origin
  uint16_t width;   ///< Width of event region
  uint16_t height;  ///< Height of event region
} TSC2046_EventTypedef;

static TSC2046_EventTypedef events[MAX_EVENTS]; ///< Registered events
static int registeredEvents; ///< Number of registered events

static uint8_t penirqAsserted; ///< Is PENIRQ low?

static void penirqCallback(void);
void TSC2046_ReadPos(uint16_t *x, uint16_t *y);

/**
 * @brief Initialize the touchscreen library.
 */
void TSC2046_Init(void) {

//  Pin mapping for touchscreen:
//  T_CLK  - PC10 (SCK)
//  T_CS   - PA15
//  T_DIN  - PA12 (MOSI)
//  T_DO   - PA11 (MISO)
//  T_IRQ  - PD2

  // initialize SPI interface
  SPI3_Init();
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

  SPI3_Select();
  SPI3_Transmit(ctrl.byte);
  SPI3_Transmit(0);
  SPI3_Transmit(0);
  SPI3_Deselect();
}
/**
 * @brief Registers a given region of the touchscreen
 * to trigger an event.
 *
 * @param x X position of starting point of region
 * @param y Y position of starting point of region
 * @param w Width of region
 * @param h Height of region
 * @param cb Callback function for event.
 * @return Current event index or -1 in case of error.
 */
int TSC2046_RegisterEvent(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
    void (*cb)(uint16_t x, uint16_t y)) {

  // if too many events
  if (registeredEvents >= MAX_EVENTS) {
    return -1;
  }

  // complete event structure
  events[registeredEvents].x = x;
  events[registeredEvents].y = y;
  events[registeredEvents].width = w;
  events[registeredEvents].height = h;
  events[registeredEvents].cb = cb;

  // update count
  registeredEvents++;
  return registeredEvents;
}
/**
 * @brief Handler for touchscreen actions.
 *
 * @details Call this function regularly in main to handle
 * touchscreen events.
 *
 */
void TSC2046_Update(void) {

  static uint32_t debounce = 0;
  static uint8_t irqReceived = 0;
  uint16_t x, y;

  if (irqReceived == 1) { // init counter
    debounce = TIMER_GetTime();
    irqReceived = 2;
  } else if (irqReceived == 2) { // debounce delay
    if (TIMER_DelayTimer(DEBOUNCE_TIME, debounce)) {
      irqReceived = 3;
      debounce = TIMER_GetTime();
      // still down?
      if (!TSC2046_HAL_ReadPenirq()) {

        TSC2046_ReadPos(&x, &y);

        for (int i = 0; i < registeredEvents; i++) {
          if ((x > events[i].x) && (x<=events[i].width+events[i].x)
              && (y > events[i].y) && (y<=events[i].height+events[i].y)) {
            events[i].cb(x, y);
          }
        }

      }
    }
  } else if (irqReceived == 3) { // wait for next measurement
    if (TIMER_DelayTimer(WAIT_TIME, debounce)) {
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
  SPI3_Select();

  // control byte
  ControlByteTypedef ctrl;
  ctrl.bits.startBit = 1;
  ctrl.bits.serDfr = DIFFERENTIAL;
  ctrl.bits.mode = MODE_12BIT;
  ctrl.bits.powerDown = PD_POWER_DOWN;
  ctrl.bits.channelSelect = MEASURE_Y;

  uint16_t tmpX, tmpY;

  // read Y
  SPI3_Transmit(ctrl.byte);
  tmpY = ((uint16_t)SPI3_Transmit(0))<<8;
  tmpY |= SPI3_Transmit(0);

  ctrl.bits.channelSelect = MEASURE_X;

  // read X
  SPI3_Transmit(ctrl.byte);
  tmpX = ((uint16_t)SPI3_Transmit(0))<<8;
  tmpX |= SPI3_Transmit(0);

  *y = tmpY >> 3;
  *x = tmpX >> 3;

  println("Data from TSC: x=%u y=%u", *x, *y);

  SPI3_Deselect();
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

/**
 * @}
 */
