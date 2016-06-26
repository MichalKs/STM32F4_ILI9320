/**
 * @file    ili9320.h
 * @brief   Library for ILI9320 TFT LCD driver.
 * @date    22 maj 2014
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

#ifndef ILI9320_H_
#define ILI9320_H_

#include <inttypes.h>

/**
 * @defgroup  ILI9320 ILI9320
 * @brief     ILI9320 TFT LCD library
 */

/**
 * @addtogroup ILI9320
 * @{
 */

void ILI9320_Initializtion(void);
void ILI9320_SetWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void ILI9320_DrawPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b);
uint16_t ILI9320_RGBDecode(uint8_t r, uint8_t g, uint8_t b);

/**
 * @}
 */

#endif /* ILI9320_H_ */
