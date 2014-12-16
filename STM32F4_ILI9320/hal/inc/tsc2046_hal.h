/**
 * @file    tsc2046_hal.h
 * @brief	  
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
#ifndef INC_TSC2046_HAL_H_
#define INC_TSC2046_HAL_H_

#include <inttypes.h>

void TSC2046_HAL_EnablePenirq(void);
void TSC2046_HAL_DisablePenirq(void);
void TSC2046_HAL_PenirqInit(void (*penirqCb)(void));
uint8_t TSC2046_HAL_ReadPenirq(void);

#endif /* INC_TSC2046_HAL_H_ */
