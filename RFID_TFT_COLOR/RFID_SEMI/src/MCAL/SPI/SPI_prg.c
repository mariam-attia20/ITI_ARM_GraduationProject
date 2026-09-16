/*
 * SPI_prg.c
 *
 *  Created on: Sep 2, 2026
 *      Author: lenovo
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"

#include "SPI_int.h"
#include "SPI_prv.h"

void MSPI_vInit(void)
{
	//sw slave
	SET_BIT(SPI1->CR1,SSM);
	//master
	SET_BIT(SPI1->CR1,SSI);
	//data
	CLR_BIT(SPI1->CR1,DFF);
	//most sign bit (MSB)
	CLR_BIT(SPI1->CR1,LSBFIRST);
	//SELCT MASTER
	SET_BIT(SPI1->CR1,MSTR);
	//POLARITY
	CLR_BIT(SPI1->CR1,CPOL);
	CLR_BIT(SPI1->CR1,CPHA);
	//ENABLE SPI (LZM L ENABLE YKON F AL A5ER)
	SET_BIT(SPI1->CR1,SPE);


}



u8 MSPI_u8Transceive(u8 A_u8Data)
{
	while(!GET_BIT(SPI1->SR , TXE));
	SPI1->DR = A_u8Data;

	while(!GET_BIT(SPI1->SR , RXNE));
	return SPI1->DR;
}



