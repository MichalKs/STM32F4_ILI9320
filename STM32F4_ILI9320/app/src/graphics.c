/**
 * @file: 	graphics.c
 * @brief:	   
 * @date: 	28 maj 2014
 * @author: Michal Ksiezopolski
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

#include "graphics.h"
#include "ili9320.h"

/**
 * @brief Color structure
 */
typedef struct {
  uint8_t r; ///< Red
  uint8_t g; ///< Green
  uint8_t b; ///< Blue
} GRAPH_ColorStruct;

typedef struct {
  uint16_t signature;
  uint32_t size;
  uint32_t reserved;
  uint32_t dataOffset;
  uint32_t headerSize;
  uint32_t width;
  uint32_t height;
  uint32_t planes;
  uint32_t bitsPerPixel;
  uint32_t compressionType;
  uint32_t imageSize;
  uint32_t resolutionH;
  uint32_t resolutionV;
  uint32_t colorsInImage;
  uint32_t importantColors;
} BMP_File;

static GRAPH_ColorStruct currentColor; ///< Global color

/**
 * @brief Initialized graphics - TFT LCD ILI9320.
 */
void GRAPH_Init(void) {
  ILI9320_Initializtion();
  ILI9320_SetWindow(0, 0, 320, 240);
}

/**
 * @brief Sets the global color variable.
 * @param r
 * @param g
 * @param b
 */
void GRAPH_SetColor(uint8_t r, uint8_t g, uint8_t b) {

  currentColor.r = r;
  currentColor.b = b;
  currentColor.g = g;
}

/**
 * @brief Draws a rectangle.
 * @param x
 * @param y
 * @param w
 * @param h
 */
void GRAPH_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {

  int i, j;
  for (i = x; i < x + w; i++) {
    for (j = y; j < y + h; j++) {
      ILI9320_DrawPixel(i, j, currentColor.r, currentColor.g, currentColor.b);
    }
  }
}
/**
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void GRAPH_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {

  int x, y, dx, dy;
  dx = x2 - x1;
  dy = y2 - y1;

  for (x = x1; x < x2; x++) {
    y = y1 + dy * (x - x1) / dx;
    ILI9320_DrawPixel(x, y, currentColor.r, currentColor.g, currentColor.b);
  }

}

