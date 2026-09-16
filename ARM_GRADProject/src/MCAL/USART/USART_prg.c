#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "USART_int.h"
#include "USART_prv.h"


void MUSART_vInit(void)
{
    /*
     * USART1
     *
     * 8 Data bits
     * No parity
     * 1 Stop bit
     * 9600 Baud
     */

    /* Oversampling by 16 */
    CLR_BIT(USART1->CR1, 15);

    /* 8 Data bits */
    CLR_BIT(USART1->CR1, 12);

    /* No parity */
    CLR_BIT(USART1->CR1, 10);

    /*
     * 9600 Baud
     *
     * This value assumes the USART1 clock
     * is configured appropriately.
     */
    USART1->BRR = (162 << 4) | 13;

    /* 1 Stop bit */
    CLR_BIT(USART1->CR2, 12);
    CLR_BIT(USART1->CR2, 13);

    /* Enable Transmitter */
    SET_BIT(USART1->CR1, 3);

    /* Enable Receiver */
    SET_BIT(USART1->CR1, 2);

    /* Enable USART1 */
    SET_BIT(USART1->CR1, 13);
}


u8 MUSART_vReceive_synch(u8 *A_pu8Byte)
{
    if(GET_BIT(USART1->SR, 5))
    {
        *A_pu8Byte = (u8)USART1->DR;

        return 1;
    }

    return 0;
}


void MUSART_vSendData(u8 A_u8Data)
{
    /* Wait until transmit data register is empty */
    while(!GET_BIT(USART1->SR, 7))
    {
    }

    USART1->DR = A_u8Data;

    /* Wait until transmission is complete */
    while(!GET_BIT(USART1->SR, 6))
    {
    }

    /* Clear TC */
    CLR_BIT(USART1->SR, 6);
}


u8 MUSART_u8ReceiveData(void)
{
    while(!GET_BIT(USART1->SR, 5))
    {
    }

    return (u8)USART1->DR;
}


void MUSART_vSendString(u8 *A_u8String)
{
    u8 L_u8Index = 0;

    while(A_u8String[L_u8Index] != '\0')
    {
        MUSART_vSendData(A_u8String[L_u8Index]);

        L_u8Index++;
    }
}


u8 MUSART_u8Retreive_USART1_DataRegister(void)
{
    return (u8)USART1->DR;
}


void MUSART_vWrite_USART1_DataRegister(u8 A_u8Data)
{
    USART1->DR = A_u8Data;
}


void MUSART_vEnable_TX_Interrupt(void)
{
    SET_BIT(USART1->CR1, 7);
}


void MUSART_vDisable_TX_Interrupt(void)
{
    CLR_BIT(USART1->CR1, 7);
}


void MUSART_vEnable_TC_Interrupt(void)
{
    SET_BIT(USART1->CR1, 6);
}


void MUSART_vDisable_TC_Interrupt(void)
{
    CLR_BIT(USART1->CR1, 6);
}


void MUSART_vEnable_RX_Interrupt(void)
{
    SET_BIT(USART1->CR1, 5);
}


void MUSART_vDisable_RX_Interrupt(void)
{
    CLR_BIT(USART1->CR1, 5);
}
