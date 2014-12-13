/**
 * @file: 	graphics.h
 * @brief:	Graphic library for TFT LCD.
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

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <inttypes.h>

void GRAPH_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void GRAPH_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void GRAPH_Init(void);
void GRAPH_SetColor(uint8_t r, uint8_t g, uint8_t b);
void GRAPH_DrawBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t lineWidth);
void GRAPH_DrawCircle(uint16_t x0, uint16_t y0, uint16_t radius);
void GRAPH_DrawFilledCircle(uint16_t x, uint16_t y, uint16_t radius);
void GRAPH_DrawString(const char* s, uint16_t x, uint16_t y);
void GRAPH_DrawChar(uint8_t c, uint16_t x, uint16_t y);
void GRAPH_SetBgColor(uint8_t r, uint8_t g, uint8_t b);
void GRAPH_ClrScreen(uint8_t r, uint8_t g, uint8_t b);
void GRAPH_DrawImage(uint16_t x, uint16_t y);
void GRAPH_DrawGraph(const uint8_t* data, uint16_t len, uint16_t x, uint16_t y);
void GRAPH_DrawBarChart(const uint8_t* data, uint16_t len, uint16_t x, uint16_t y, uint16_t width);

#endif /* GRAPHICS_H_ */
