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

/**
 * @brief Structure for reading BMP files
 */
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
  // window occupies whole LCD screen
  ILI9320_SetWindow(0, 0, 320, 240);
}

/**
 * @brief Sets the global color variable.
 *
 * @details All subsequent objects will be drawn using this color.
 *
 * @param r Red
 * @param g Green
 * @param b Blue
 */
void GRAPH_SetColor(uint8_t r, uint8_t g, uint8_t b) {

  currentColor.r = r;
  currentColor.b = b;
  currentColor.g = g;
}

/**
 * @brief Draws a rectangle (filled).
 * @param x X coordinate of start point
 * @param y Y coordinate of start point
 * @param w Width
 * @param h Height
 */
void GRAPH_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {

  int i, j;
  // Fill rectangle with color
  for (i = x; i < x + w; i++) {
    for (j = y; j < y + h; j++) {
      ILI9320_DrawPixel(i, j, currentColor.r, currentColor.g, currentColor.b);
    }
  }
}
/**
 * @brief Draws a box (empty rectangle).
 * @param x X coordinate of start point
 * @param y Y coordinate of start point
 * @param w Width
 * @param h Height
 * @param lineWidth Width of borders
 */
void GRAPH_DrawBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t lineWidth) {

  // Draw borders
  GRAPH_DrawRectangle(x, y, lineWidth, h);
  GRAPH_DrawRectangle(x+lineWidth, y, w-2*lineWidth, lineWidth);
  GRAPH_DrawRectangle(x+w-lineWidth, y, lineWidth, h);
  GRAPH_DrawRectangle(x+lineWidth, y+h-lineWidth, w-2*lineWidth, lineWidth);

}
/**
 * @brief Draws a circle
 * @param x0 Center X coordinate.
 * @param y0 Center Y coordinate.
 * @param radius Circle radius.
 */
void GRAPH_DrawCircle(uint16_t x, uint16_t y, uint16_t radius) {

  int newX = radius;
  int newY = 0;
  int error = 1-newX;

  while(newX >= newY) {
    ILI9320_DrawPixel(newX + x, newY + y,
        currentColor.r, currentColor.g, currentColor.b);
    ILI9320_DrawPixel(newY + x, newX + y,
        currentColor.r, currentColor.g, currentColor.b);
    ILI9320_DrawPixel(-newX + x, newY + y,
        currentColor.r, currentColor.g, currentColor.b);
    ILI9320_DrawPixel(-newY + x, newX + y,
        currentColor.r, currentColor.g, currentColor.b);
    ILI9320_DrawPixel(-newX + x, -newY + y,
        currentColor.r, currentColor.g, currentColor.b);
    ILI9320_DrawPixel(-newY + x, -newX + y,
        currentColor.r, currentColor.g, currentColor.b);
    ILI9320_DrawPixel(newX + x, -newY + y,
        currentColor.r, currentColor.g, currentColor.b);
    ILI9320_DrawPixel(newY + x, -newX + y,
        currentColor.r, currentColor.g, currentColor.b);

    newY++;

    if (error<0) {
      error += 2 * newY + 1;
    } else {
      newX--;
      error += 2 * (newY - newX + 1);
    }
  }
}
/**
 * @brief This function draws a line.
 *
 * @param x1 Starting point X coordinate
 * @param y1 Starting point Y coordinate
 * @param x2 End point X coordinate
 * @param y2 End point Y coordinate
 */
void GRAPH_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {

  int x, y, dx, dy;
  dx = x2 - x1; // Change in X
  dy = y2 - y1; // Change in Y

  for (x = x1; x < x2; x++) {
    y = y1 + dy * (x - x1) / dx;
    ILI9320_DrawPixel(x, y, currentColor.r, currentColor.g, currentColor.b);
  }
}

