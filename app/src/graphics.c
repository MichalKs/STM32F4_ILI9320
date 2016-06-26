/**
 * @file    graphics.c
 * @brief   Graphic library for TFT LCD.
 * @date    28 maj 2014
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

#include <graphics.h>
#include <ili9320.h>
#include <string.h>
#include <example_bmp.h>
#include <font_8x16.h>
#include <math.h>

/**
 * @addtogroup GRAPHICS
 * @{
 */

/**
 * @brief Structure containing information about
 * an image.
 *
 * @details The image is assumed to be structured in the following way.
 * Image data starts in the top right corner. The first three bytes are
 * the RGB bytes of the last column of the first row, next three bytes
 * are the second to last column of row 1, etc. After column 1 is reached
 * the next row starts.
 *
 */
typedef struct {
  const uint8_t* data;    ///< Image data
  uint16_t rows;          ///< Number of pixel rows
  uint16_t columns;       ///< Number of pixel columns
  uint8_t bytesPerPixel;  ///< Number of bytes per pixel
} GRAPH_ImageStruct;


/**
 * @brief Currently set font.
 */
static GRAPH_FontStruct currentFont;
/**
 * @brief Example image to be drawn on screen.
 */
static GRAPH_ImageStruct displayedImage = {
    example_bmp,
    192,
    256,
    3
};

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

static GRAPH_ColorStruct currentColor;    ///< Global color
static GRAPH_ColorStruct currentBgColor;  ///< Global background color


/**
 * @brief Initialized graphics - TFT LCD ILI9320.
 */
void GRAPH_Init(void) {
  ILI9320_Initializtion();
  // window occupies whole LCD screen
  ILI9320_SetWindow(0, 0, 320, 240);
  GRAPH_ClrScreen(0, 0, 0); // black screen on startup
}
/**
 * @brief Clears the screen with given color.
 */
void GRAPH_ClrScreen(uint8_t r, uint8_t g, uint8_t b) {

  GRAPH_ColorStruct tmp = currentColor; // save current color

  currentColor.r = r;
  currentColor.b = b;
  currentColor.g = g;

  GRAPH_DrawRectangle(0, 0, 320, 240);

  currentColor = tmp;
}
/**
 * @brief Sets the currently used font.
 *
 * @details This function should be called before attempting
 * to write a string to the LCD.
 *
 * @param font Font information structure.
 */
void GRAPH_SetFont(GRAPH_FontStruct font) {

  currentFont = font;
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
 * @brief Sets the global background color variable.
 *
 * @param r Red
 * @param g Green
 * @param b Blue
 */
void GRAPH_SetBgColor(uint8_t r, uint8_t g, uint8_t b) {

  currentBgColor.r = r;
  currentBgColor.b = b;
  currentBgColor.g = g;
}
/**
 * @brief Draws an image on screen.
 * @param x X coordinate of top right corner.
 * @param y Y coordinate of top right corner.
 *
 * TODO Make function more general.
 */
void GRAPH_DrawImage(uint16_t x, uint16_t y) {

  int r, g, b;
  int pos;

  for (int i = 0; i < displayedImage.rows; i++) { // rows
    for (int j = 0; j < displayedImage.columns; j++) { // columns
      pos = (i*displayedImage.columns+j)*displayedImage.bytesPerPixel;
      r = displayedImage.data[pos];
      g = displayedImage.data[pos+1];
      b = displayedImage.data[pos+2];
      ILI9320_DrawPixel(j+x, i+y, r, g, b);
    }
  }

}
/**
 * @brief Draws a character on screen.
 * @param c Character to draw (ASCII code)
 * @param x X coordinate of character
 * @param y T coordinate of character
 */
void GRAPH_DrawChar(uint8_t c, uint16_t x, uint16_t y) {

  // no font set
  if (currentFont.data == 0) {
    return;
  }

  const uint16_t row = c - currentFont.firstChar; // Font usually skips first chars (useless)
  const uint16_t bitsPerByte = 8;

  // if nonexisting char
  if (row >= currentFont.numberOfChars) {
    return;
  }

  const uint16_t pos = currentFont.columnCount *
      currentFont.bytesPerColumn * row; // first byte of row

  const uint8_t* ptr = currentFont.data;

  uint16_t bitmask;

  for (int i = 0; i < currentFont.columnCount; i++) { // for 21 columns
    for (int j = 0; j < currentFont.bytesPerColumn; j++) { // for 5 bytes per column
      bitmask = 0x01; // start from lowest bit
      for (int k = 0; k < bitsPerByte; k++, bitmask <<= 1) { // for 8 bits in byte
        if (ptr[pos + i * currentFont.bytesPerColumn + j] & bitmask) {
          ILI9320_DrawPixel(x+j*bitsPerByte+k, y+i,
              currentColor.r, currentColor.g, currentColor.b);
        } else {
          ILI9320_DrawPixel(x+j*bitsPerByte+k, y+i,
              currentBgColor.r, currentBgColor.g, currentBgColor.b);
        }
      }
    }
  }
}
/**
 * @brief Writes a string on the LCD
 * @param s String to write
 * @param x X coordinate
 * @param y Y coordinate
 * TODO Enable drawing vertical and horizontal strings.
 */
void GRAPH_DrawString(const char* s, uint16_t x, uint16_t y) {

  uint16_t len = strlen(s);

  // skip columnCount pixel columns for next char
  for (int i = 0; i < len; i++, y+=currentFont.columnCount) {
    GRAPH_DrawChar(s[i], x, y);
  }
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
 * @brief Draws a graph portraying data (measurements, etc.).
 * @param data Buffer for displayed data.
 * @param len Length of data vector.
 * @param x X coordinate of start point
 * @param y Y coordinate of start point
 *
 * TODO Add graph scaling. Add axes and their
 * descriptions, graph title.
 *
 */
void GRAPH_DrawGraph(const uint8_t* data, uint16_t len, uint16_t x, uint16_t y) {

  const uint16_t xOffset = 30; // offset for axis and description
  x += xOffset;

  const uint16_t yOffset = 30; // offset for axis and description
  y += yOffset;

  const uint16_t maxDataLen = 320 - xOffset - 20;

  GRAPH_FontStruct tmp = currentFont; // save current font

  GRAPH_SetFont(font8x16Info);
  // X axis description
  GRAPH_DrawString("Voltage [V]", 5, 50);
  // X axis
  GRAPH_DrawLine(x-2, y-2, x-2, 230);
  GRAPH_DrawLine(x-2, 230, x-12, 220);
  GRAPH_DrawLine(x-2, 230, x+8, 220);
  // Y axis
  GRAPH_DrawLine(x-2, y-2, 310, y-2);
  GRAPH_DrawLine(310, y-2, 300, y-12);
  GRAPH_DrawLine(310, y-2, 300, y+8);

  if (len > maxDataLen)
    len = maxDataLen;

  for (int i = 0; i < len; i++) {
    // draw pixels up and down to make line more visible
    ILI9320_DrawPixel(x+i,y+data[i]-1,
        currentColor.r, currentColor.g, currentColor.b);
    ILI9320_DrawPixel(x+i,y+data[i],
        currentColor.r, currentColor.g, currentColor.b);
    ILI9320_DrawPixel(x+i,y+data[i]+1,
        currentColor.r, currentColor.g, currentColor.b);
  }

  GRAPH_SetFont(tmp); // restore font
}
/**
 * @brief Draws a bar chart portraying data (measurements, etc.).
 * @param data Buffer for displayed data.
 * @param len Length of data vector.
 * @param x X coordinate of start point
 * @param y Y coordinate of start point
 * @param width Width of single bar.
 *
 * TODO Add graph scaling.
 *
 */
void GRAPH_DrawBarChart(const uint8_t* data, uint16_t len,
    uint16_t x, uint16_t y, uint16_t width) {

  uint16_t pos = x;

  const uint8_t space = 5; // space between bars

  for (int i = 0; i < len; i++, pos+=width+space) {
    // draw pixels up and down to make line more visible
    GRAPH_DrawRectangle(pos, 0, width, data[i]);
  }
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
 * @brief Draws a fille circle
 * @param x0 Center X coordinate.
 * @param y0 Center Y coordinate.
 * @param radius Circle radius.
 * TODO Change implmentation - currenly not all pixels drawn
 */
void GRAPH_DrawFilledCircle(uint16_t x, uint16_t y, uint16_t radius) {

  while (radius--) {
    GRAPH_DrawCircle(x,y,radius);
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

  int dx = x2>x1 ? (x2-x1) : (x1-x2); // slope
  int sx = x1<x2 ? 1 : -1; // sign
  int dy = y2>y1 ? (y2-y1) : (y1-y2); // slope
  int sy = y1<y2 ? 1 : -1; // sign

  int err = (dx>dy ? dx : -dy)/2; // error
  int tmpErr;

  for(;;) {

    ILI9320_DrawPixel(x1, y1, currentColor.r, currentColor.g, currentColor.b);

    // if end of line
    if (x1==x2 && y1==y2) {
      break;
    }

    tmpErr = err;

    if (tmpErr >-dx) {
      err -= dy;
      x1 += sx;
    }

    if (tmpErr < dy) {
      err += dx;
      y1 += sy;
    }
  }
//  double x, y, dx, dy;
//  dx = x2 - x1; // Change in X
//  dy = y2 - y1; // Change in Y
//
//  for (x = x1; x < x2; x++) {
//    y = y1 + dy * (x - x1) / dx;
//    ILI9320_DrawPixel(x, y, currentColor.r, currentColor.g, currentColor.b);
//  }
}

/**
 * @}
 */
