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


typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} GRAPH_ColorStruct;

static GRAPH_ColorStruct currentColor;

void GRAPH_Init(void) {
	ILI9320_Initializtion();
	ILI9320_SetWindow(0,0,320,240);
}


void GRAPH_SetColor(uint8_t r, uint8_t g, uint8_t b) {

	currentColor.r = r;
	currentColor.b = b;
	currentColor.g = g;
}


void GRAPH_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {

	int i,j;
	for (i = x; i < x + w; i++) {
		for (j = y; j < y + h; j++) {
			ILI9320_DrawPixel(i, j, currentColor.r, currentColor.g, currentColor.b);
		}

	}
}

void GRAPH_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {

	int x, y, dx, dy;
	dx = x2 - x1;
	dy = y2 - y1;
	for (x = x1; x < x2; x++) {
		y = y1 + dy * (x - x1) / dx;
		ILI9320_DrawPixel(x, y, currentColor.r, currentColor.g, currentColor.b);


	}

}


