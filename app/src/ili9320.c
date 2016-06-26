/**
 * @file   ili9320.c
 * @brief  Library for ILI9320 TFT LCD driver.
 * @date   22 maj 2014
 * @author Michal Ksiezopolski
 * 
 * @details The ILI9320 driver can handle 512 pixels horizontally
 * by 256 pixels vertically. My display has only 320x240 pixels,
 * however - the data wraps around.
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

#include <ili9320.h>
#include <timers.h>
#include <stdio.h>
#include <ili9320_hal.h>

/**
 * @addtogroup ILI9320
 * @{
 */

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

uint16_t ILI9320_RGBDecode(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Initialize the ILI9320 TFT LCD driver.
 */
void ILI9320_Initializtion(void) {

  ILI9320_HAL_HardInit(); // GPIO and FSMC init

  // Reset the LCD
  ILI9320_HAL_ResetOff();
  TIMER_Delay(50);
  ILI9320_HAL_ResetOn();
  TIMER_Delay(50);
  ILI9320_HAL_ResetOff();
  TIMER_Delay(50);

  ILI9320_HAL_WriteReg(ILI9320_START_OSCILLATION, 0x0001);
  TIMER_Delay(20);

  // Read LCD ID
  unsigned int id;
  id = ILI9320_HAL_ReadReg(ILI9320_READ_ID);

  printf("ID TFT LCD = %x\r\n", id);

  // Add more LCD init codes here
  if (id == 0x9320) {

    ILI9320_HAL_WriteReg(ILI9320_DRIVER_OUTPUT, 0x0100); // SS = 1 - coordinates from left to right
    ILI9320_HAL_WriteReg(ILI9320_DRIVING_WAVE, 0x0700);  // Line inversion
    ILI9320_HAL_WriteReg(ILI9320_ENTRY_MODE, 0x1018);    //
    ILI9320_HAL_WriteReg(ILI9320_RESIZE, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_DISP1, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_DISP2, 0x0202); // two lines back porch, two line front porch
    ILI9320_HAL_WriteReg(ILI9320_DISP3, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_DISP4, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_RGB_DISP1, 0x0001);
    ILI9320_HAL_WriteReg(ILI9320_FRAME_MARKER, 0x0000); // 0th line for frame marker
    ILI9320_HAL_WriteReg(ILI9320_RGB_DISP2, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_DISP1, 0x0101);
    ILI9320_HAL_WriteReg(ILI9320_POWER1, 0x10c0);
    ILI9320_HAL_WriteReg(ILI9320_POWER2, 0x0007);
    ILI9320_HAL_WriteReg(ILI9320_POWER3, 0x0110);
    ILI9320_HAL_WriteReg(ILI9320_POWER4, 0x0b00);
    ILI9320_HAL_WriteReg(ILI9320_POWER7, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_FRAME_RATE, 0x4010);

    // Set window
    ILI9320_HAL_WriteReg(ILI9320_HOR_ADDR_START, 0);
    ILI9320_HAL_WriteReg(ILI9320_HOR_ADDR_END, 239);
    ILI9320_HAL_WriteReg(ILI9320_VER_ADDR_START, 0);
    ILI9320_HAL_WriteReg(ILI9320_VER_ADDR_END, 319);

    ILI9320_HAL_WriteReg(ILI9320_DRIVER_OUTPUT2, 0x2700);
    ILI9320_HAL_WriteReg(ILI9320_BASE_IMAGE, 0x0001);
    ILI9320_HAL_WriteReg(ILI9320_VERTICAL_SCROLL, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_PARTIAL1_POS, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_PARTIAL1_START, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_PARTIAL1_END, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_PARTIAL2_POS, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_PARTIAL2_START, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_PARTIAL2_END, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_PANEL_INTERFACE1, 0x0010);
    ILI9320_HAL_WriteReg(ILI9320_PANEL_INTERFACE2, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_PANEL_INTERFACE3, 0x0001);
    ILI9320_HAL_WriteReg(ILI9320_PANEL_INTERFACE4, 0x0110);
    ILI9320_HAL_WriteReg(ILI9320_PANEL_INTERFACE5, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_PANEL_INTERFACE6, 0x0000);
    ILI9320_HAL_WriteReg(ILI9320_DISP1, 0x0173);

  }

  TIMER_Delay(100);
}
/**
 * @brief Convert RGB value to ILI9320 format.
 * @param r Red
 * @param g Green
 * @param b Blue
 * @return Converted value of color.
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

  ILI9320_HAL_WriteReg(ILI9320_HOR_GRAM_ADDR, y);
  ILI9320_HAL_WriteReg(ILI9320_VER_GRAM_ADDR, x);

}
/**
 * @brief Draws a pixel on the LCD.
 * @param x X coordinate of pixel.
 * @param y Y coordinate of pixel.
 * @param r Red color value.
 * @param g Green color value.
 * @param b Blue color value.
 */
void ILI9320_DrawPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {

  ILI9320_SetCursor(x, y);
  ILI9320_HAL_WriteReg(ILI9320_WRITE_TO_GRAM, ILI9320_RGBDecode(r, g, b));
}
/**
 * @brief Set work window to draw data.
 * @param x X coordinate of start point.
 * @param y Y coordinate of start point.
 * @param width Width of window.
 * @param height Height of window.
 */
void ILI9320_SetWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {

  ILI9320_SetCursor(x, y);
  ILI9320_HAL_WriteReg(ILI9320_HOR_ADDR_START, y);
  ILI9320_HAL_WriteReg(ILI9320_HOR_ADDR_END, y + height - 1);
  ILI9320_HAL_WriteReg(ILI9320_VER_ADDR_START, x);
  ILI9320_HAL_WriteReg(ILI9320_VER_ADDR_END, x + width - 1);
}

/**
 * @}
 */
