/**
 * @file    gui.h
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
#ifndef INC_GUI_H_
#define INC_GUI_H_

/**
 * @defgroup  GUI GUI
 * @brief     Graphical user interface library for touchscreen and TFT LCD
 */

/**
 * @addtogroup GUI
 * @{
 */

void GUI_AddButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
    void (*cb)(uint16_t x, uint16_t y), const char* text);
void GUI_Init(void);

/**
 * @}
 */

#endif /* INC_GUI_H_ */
