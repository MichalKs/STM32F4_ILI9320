/**
 * @file    spi3.c
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

#include <spi3.h>
#include <stm32f4xx.h>

/**
 * @addtogroup SPI3
 * @{
 */

/**
 * @brief Initialize SPI3 and SS pin.
 */
void SPI3_Init(void) {

  // Enable GPIO clock for SPI pins
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  /*
   * Initialize pins as alternate function, push-pull.
   * PC10 = SCK
   * PC11 = MISO
   * PC12 = MOSI
   * PA15 = SS
   */
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_PuPd   = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStruct);

  // Enable alternate functions of pins
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

  // Software chip select pin, PA15 = SS
  GPIO_InitStruct.GPIO_Pin    = GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_PuPd   = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_SetBits(GPIOA, GPIO_Pin_15); // Set SS line

  // Enable SPI3 clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

  SPI_InitTypeDef SPI_InitStruct;

  SPI_InitStruct.SPI_Direction          = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct.SPI_Mode               = SPI_Mode_Master;
  SPI_InitStruct.SPI_DataSize           = SPI_DataSize_8b;
  SPI_InitStruct.SPI_CPOL               = SPI_CPOL_Low;
  SPI_InitStruct.SPI_CPHA               = SPI_CPHA_1Edge;
  SPI_InitStruct.SPI_NSS                = SPI_NSS_Soft; // software chip select
  SPI_InitStruct.SPI_BaudRatePrescaler  = SPI_BaudRatePrescaler_256;
  SPI_InitStruct.SPI_FirstBit           = SPI_FirstBit_MSB;
  SPI_InitStruct.SPI_CRCPolynomial      = 7;
  SPI_Init(SPI3, &SPI_InitStruct);

  SPI_CalculateCRC(SPI3, DISABLE);
  SPI_Cmd(SPI3, ENABLE); // enable SPI3

}
/**
 * @brief Select chip.
 */
void SPI3_Select(void) {

  GPIO_ResetBits(GPIOA, GPIO_Pin_15); // Reset SS line

}
/**
 * @brief Deselect chip.
 */
void SPI3_Deselect(void) {

  GPIO_SetBits(GPIOA, GPIO_Pin_15); // Set SS line
}
/**
 * @brief Transmit data on SPI3
 * @param data Sent data
 * @return Received data
 * @warning Blocking function!
 */
uint8_t SPI3_Transmit(uint8_t data) {

  uint8_t i;

  // Loop while transmit register in not empty
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);

  SPI_I2S_SendData(SPI3, data); // Send byte (start transmit)

  // Wait for new data (transmit end)
  while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);

  i = SPI_I2S_ReceiveData(SPI3); // Received data

  return i;
}
/**
 * @brief Send multiple data on SPI3.
 * @param buf Buffer to send.
 * @param len Number of bytes to send.
 * @warning Blocking function!
 */
void SPI3_SendBuffer(uint8_t* buf, uint32_t len) {

  while (len--) {
    SPI3_Transmit(*buf++);
  }
}
/**
 * @brief Read multiple data on SPI3.
 * @param buf Buffer to place read data.
 * @param len Number of bytes to read.
 * @warning Blocking function!
 */
void SPI3_ReadBuffer(uint8_t* buf, uint32_t len) {

  while (len--) {
    *buf++ = SPI3_Transmit(0xff);
  }
}
/**
 * @brief Transmit multiple data on SPI3.
 * @param rx_buf Receive buffer.
 * @param tx_buf Transmit buffer.
 * @param len Number of bytes to transmit.
 */
void SPI3_TransmitBuffer(uint8_t* rx_buf, uint8_t* tx_buf, uint32_t len) {

  while (len--) {
    *rx_buf = SPI3_Transmit(*tx_buf);
    tx_buf++;
    rx_buf++;
  }
}

/**
 * @}
 */
