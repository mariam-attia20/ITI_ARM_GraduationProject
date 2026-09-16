/*
 * UART_prg.c
 *
 *  Created on: 31 Aug 2026
 *      Author: LOQ
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"


#include "UART_int.h"
#include "UART_prv.h"

u8   G_u8TxData ;
u8   G_Buffer[50];
u8 *G_u8PtrData = NULL ;
u8 G_u8Len = 0;
u8 G_u8Indx = 0;

void MUART_vInt(void)
{
	//Data length
	CLR_BIT(USART1->CR1 , 12);
	//No parity
	CLR_BIT(USART1->CR1 , 10);
	//baud rate 9600
    //mantesa = 52
	//friction = 2
	USART1 ->BRR = (52<<4)|1 ;
	//stop
	CLR_BIT(USART1->CR2, 12);
	CLR_BIT(USART1->CR2 , 13);
	//enable Transmiter
	SET_BIT(USART1->CR1 , 3);
	//enable reciver
	SET_BIT(USART1->CR1 , 2);
	//enable uart
	SET_BIT(USART1->CR1 , 13);

}

void MUART_vSendData(u8 A_Data)
{
	while(GET_BIT(USART1->SR , 7)!=1)
		{
		}
	USART1->DR = A_Data;
	while(GET_BIT(USART1->SR , 6)!=1)
	{
	}
	CLR_BIT(USART1->SR , 6);

}

void MUART_vSendStg(u8 * A_u8PtrData )
{
	if(G_u8PtrData[G_u8Indx] == '\0'){

		G_u8PtrData = A_u8PtrData;
		G_u8Indx= 0;
		MUART_vEnable_TX_Interrupt();
	}

}

void MUART_vSendChar(u8 A_Data)
{
	G_u8TxData = A_Data;
	MUART_vEnable_TX_Interrupt();

}

u8 MUART_u8ReceiveData(void)
{
	while(GET_BIT(USART1->SR , 5)!=1)
	{
	}
	return USART1->DR ;

}

void MUART_vEnable_TX_Interrupt(void)
{
	SET_BIT(USART1->CR1 , 7);
}
void MUART_vDisable_TX_Interrupt(void)
{
	CLR_BIT(USART1->CR1 , 7);
}

void MUART_vEnable_RX_Interrupt(void)
{
	SET_BIT(USART1->CR1 , 5);
}
void MUART_vDisable_RX_Interrupt(void)
{
	CLR_BIT(USART1->CR1 , 5);
}

void MUART_vEnable_TC_Interrupt(void)
{
	SET_BIT(USART1->CR1 , 6);
}
void MUART_vDisable_TC_Interrupt(void)
{
	CLR_BIT(USART1->CR1 , 6);
}

void USART1_IRQHandler(void)
{
	if((GET_BIT(USART1->SR , 7)==1) &&(GET_BIT(USART1 ->CR1 , 7)==1))
	{
	   if(G_u8PtrData[G_u8Indx] != '\0')
	   {
		   USART1 ->DR = G_u8PtrData[G_u8Indx];
		   G_u8Indx++;
	   }else{
		   MUART_vDisable_TX_Interrupt();
		   MUART_vEnable_TC_Interrupt();
	   }
	}
	if( ((GET_BIT(USART1->SR , 6))==1) && (GET_BIT(USART1 ->CR1 , 6)==1))
	{
		//clear tc bit
		CLR_BIT(USART1->SR , 6);
		MUART_vDisable_TC_Interrupt();
	}

}
