/**
 * @file    spi3.h
 * @brief	  
 * @date    17 gru 2014
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
#ifndef INC_SPI3_H_
#define INC_SPI3_H_

#include <inttypes.h>

/**
 * @defgroup  SPI3 SPI3
 * @brief     SPI3 control functions
 */

/**
 * @addtogroup SPI3
 * @{
 */

uint8_t SPI3_Transmit       (uint8_t data);
void    SPI3_Init           (void);
void    SPI3_Select         (void);
void    SPI3_Deselect       (void);
void    SPI3_ReadBuffer     (uint8_t* buf, uint32_t len);
void    SPI3_SendBuffer     (uint8_t* buf, uint32_t len);
void    SPI3_TransmitBuffer (uint8_t* rx_buf, uint8_t* tx_buf, uint32_t len);

/**
 * @}
 */

#endif /* INC_SPI3_H_ */
