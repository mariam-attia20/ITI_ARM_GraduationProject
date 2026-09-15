/************************************************/
/* Layer   : MCAL                               */
/* SWC     : SPI1 (Master, Mode 0, MSB first)   */
/* File    : SPI_interface.h                    */
/************************************************/
#ifndef SPI_INTERFACE_H
#define SPI_INTERFACE_H

/*
 * SPI1 pin map (software NSS - chip select is a normal GPIO):
 *   PA5 -> SCK   (Alternate Function Push-Pull)
 *   PA6 -> MISO  (Input Floating)
 *   PA7 -> MOSI  (Alternate Function Push-Pull)
 */
void MSPI1_voidInit(void);

/* Sends one byte and returns the byte shifted in at the same time */
u8   MSPI1_u8Transceive(u8 Copy_u8Data);

#endif
