/**
 * @file    tsc2046.h
 * @brief   TSC2046 touchscreen library
 * @date    14 gru 2014
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
#ifndef INC_TSC2046_H_
#define INC_TSC2046_H_

#include <inttypes.h>

/**
 * @defgroup  TSC2046 TSC2046
 * @brief     TSC2046 touchscreen library
 */

/**
 * @addtogroup TSC2046
 * @{
 */

void TSC2046_Init(void);
void TSC2046_Update(void);
int TSC2046_RegisterEvent(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
    void (*cb)(uint16_t x, uint16_t y));

/**
 * @}
 */

#endif /* INC_TSC2046_H_ */
