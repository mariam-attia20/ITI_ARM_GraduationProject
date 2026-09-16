#ifndef MCAL_USART_USART_INT_H_
#define MCAL_USART_USART_INT_H_

#include "../../LIB/STD_TYPES.h"

void MUSART_vInit(void);

void MUSART_vSendData(u8 A_u8Data);

u8 MUSART_u8ReceiveData(void);

u8 MUSART_vReceive_synch(u8 *A_pu8Byte);

void MUSART_vSendString(u8 *A_pu8String);


/* Interrupts */

void MUSART_vEnable_TX_Interrupt(void);
void MUSART_vDisable_TX_Interrupt(void);

void MUSART_vEnable_TC_Interrupt(void);
void MUSART_vDisable_TC_Interrupt(void);

void MUSART_vEnable_RX_Interrupt(void);
void MUSART_vDisable_RX_Interrupt(void);


/* USART1 Data Register */

u8 MUSART_u8Retreive_USART1_DataRegister(void);

void MUSART_vWrite_USART1_DataRegister(u8 A_u8Data);


/* Callback */

void MUSART_vUSARTCallBack(u8 A_u8USARTNo, void (*Fptr)(void));

#endif
