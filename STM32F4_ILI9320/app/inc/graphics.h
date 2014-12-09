/**
 * @file: 	graphics.h
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

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <stm32f4xx.h>

void GRAPH_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void GRAPH_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void GRAPH_Init(void);
void GRAPH_SetColor(uint8_t r, uint8_t g, uint8_t b);

#endif /* GRAPHICS_H_ */
