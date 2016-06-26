/**
 * @file    gui.c
 * @brief   Graphical user interface library for touchscreen and TFT LCD
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


#include <graphics.h>
#include <tsc2046.h>
#include <font_8x16.h>

/**
 * @addtogroup GUI
 * @{
 */

static void GUI_ConvertLCD2TSC(uint16_t *x, uint16_t *y, uint16_t *w, uint16_t *h);

/**
 * @brief Initialize GUI.
 */
void GUI_Init(void) {

  TSC2046_Init(); // initialize touchscreen
  GRAPH_Init();
  GRAPH_SetColor(0xff, 0xff, 0x00);
  GRAPH_SetBgColor(0xff, 0x00, 0x00);
  GRAPH_SetFont(font8x16Info);
}


/**
 * @brief Adds a button to the GUI.
 *
 * @details All coordinates as per LCD (not TSC).
 *
 * @param x X coordinate of button origin.
 * @param y Y coordinate of button origin.
 * @param w Width of button
 * @param h Height of button
 * @param cb Callback for button press event.
 * @param text Description of button (shown on screen).
 */
void GUI_AddButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
    void (*cb)(uint16_t x, uint16_t y), const char* text) {

  GRAPH_DrawRectangle(x,y,w,h);

  // TODO Derive position of button text from string and font size
  GRAPH_DrawString(text, x+w/4, y+h/4);

  GUI_ConvertLCD2TSC(&x, &y, &w, &h);

  TSC2046_RegisterEvent(x, y, w, h, cb);

}
/**
 * @brief Draws a label on screen
 * @param x X coordinate of label origin.
 * @param y Y coordinate of label origin.
 * @param w Maximum width of label
 * @param h Maximum height of label
 * @param text Label contents
 */
void GUI_AddLabel(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
    const char* text) {

}

/**
 * @brief Converts LCD coordinates (320x240) to TSC coordinates.
 *
 * @details The axes are interchanged between the two devices in
 * my setting. The X axis on the LCD corresponds to -Y on the TSC
 * and the Y axis on the LCD to the X axis on the TSC.
 *
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h height
 */
static void GUI_ConvertLCD2TSC(uint16_t *x, uint16_t *y, uint16_t *w, uint16_t *h) {

  uint16_t tmpX, tmpY, tmpW, tmpH;
  uint16_t startX, startY;

  startY = *x + *w;
  startX = *y;
  tmpW = *h;
  tmpH = *w;

  const uint16_t lcdWidth = 320;
  const uint16_t lcdHeight = 240;

  // basically I derived those manually
  // by analyzing touchscreen readings

  const uint16_t tscMaxY = 3800;
  const uint16_t tscMinY = 400;
  const uint16_t tscMaxX = 3700;
  const uint16_t tscMinX = 300;

  const uint16_t tscDX = tscMaxX - tscMinX;
  const uint16_t tscDY = tscMaxY - tscMinY;

  // Y axis is inverted X axis of the LCD
  tmpY = tscMaxY - startY * tscDY/lcdWidth;
  // X axis is Y axis of the LCD
  tmpX = tscMinX + startX * tscDX/lcdHeight;

  *y = tmpY;
  *x = tmpX;

  *h = tmpH * tscDY/lcdWidth;
  *w = tmpW * tscDX/lcdHeight;

}

/**
 * @}
 */
