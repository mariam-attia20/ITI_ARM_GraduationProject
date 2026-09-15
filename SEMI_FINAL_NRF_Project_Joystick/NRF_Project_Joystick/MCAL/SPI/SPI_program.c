/************************************************/
/* Layer   : MCAL                               */
/* SWC     : SPI1                               */
/* File    : SPI_program.c                      */
/************************************************/
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../RCC/RCC_interface.h"
#include "../GPIO/GPIO_interface.h"
#include "SPI_interface.h"
#include "SPI_private.h"

void MSPI1_voidInit(void)
{
    /* 1- Clocks */
    MRCC_voidEnableClock(RCC_APB2, RCC_APB2_IOPA);
    MRCC_voidEnableClock(RCC_APB2, RCC_APB2_SPI1);

    /* 2- Pins */
    MGPIO_voidSetPinMode(GPIO_PORTA, GPIO_PIN5, GPIO_AF_PP_10MHZ);      /* SCK  */
    MGPIO_voidSetPinMode(GPIO_PORTA, GPIO_PIN6, GPIO_INPUT_FLOATING);   /* MISO */
    MGPIO_voidSetPinMode(GPIO_PORTA, GPIO_PIN7, GPIO_AF_PP_10MHZ);      /* MOSI */

    /* 3- Control register
     *    Master, CPOL = 0, CPHA = 0  (SPI mode 0 - required by nRF24L01)
     *    MSB first, 8-bit frame
     *    Software slave management (SSM = 1, SSI = 1)
     *    Baud rate = fPCLK2 / 16
     */
    SPI1->CR1 = 0;
    SPI1->CR1 |= (1u << 2);      /* MSTR                  */
    SPI1->CR1 |= (1u << 3);      /* BR = 001 -> fPCLK/4   */
    SPI1->CR1 |= (1u << 8);      /* SSI  = 1              */
    SPI1->CR1 |= (1u << 9);      /* SSM  = 1              */
    SPI1->CR2 = 0;
    SPI1->CR1 |= (1u << 6);      /* SPE  = 1 (enable)     */
}

u8 MSPI1_u8Transceive(u8 Copy_u8Data)
{
    while (GET_BIT(SPI1->SR, SPI_SR_TXE)  == 0);
    SPI1->DR = Copy_u8Data;
    while (GET_BIT(SPI1->SR, SPI_SR_RXNE) == 0);
    return (u8)SPI1->DR;
}
