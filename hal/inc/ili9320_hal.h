/**
 * @file: 	ili9320_hal.h
 * @brief:	Hardware abstraction layer for ILI9320
 * @date: 	9 gru 2014
 * @author: Michal Ksiezopolski
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
#ifndef INC_ILI9320_HAL_H_
#define INC_ILI9320_HAL_H_


uint16_t ILI9320_HAL_ReadReg(uint16_t reg);
void ILI9320_HAL_HardInit(void);
void ILI9320_HAL_WriteReg(uint16_t reg, uint16_t data);
void ILI9320_HAL_ResetOn(void);
void ILI9320_HAL_ResetOff(void);

#endif /* INC_ILI9320_HAL_H_ */
