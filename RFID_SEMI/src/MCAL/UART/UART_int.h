/*
 * UART_int.h
 *
 *  Created on: 31 Aug 2026
 *      Author: LOQ
 */

#ifndef MCAL_UART_UART_INT_H_
#define MCAL_UART_UART_INT_H_


void MUART_vInt(void);
void MUART_vSendData(u8 A_Data);
u8 MUART_u8ReceiveData(void);
u8 MUART_u8Receive_synch(u8* A_pu8Byte);
void MUART_vSendChar(u8 A_Data);

//interrupt
void MUART_vEnable_TX_Interrupt(void);
void MUART_vDisable_TX_Interrupt(void);
void MUART_vEnable_TC_Interrupt(void);
void MUART_vDisable_TC_Interrupt(void);
void MUART_vEnable_RX_Interrupt(void);
void MUART_vDisable_RX_Interrupt(void);

//todo
void MUSART_vSendString(char* A_u8PtrStr);
void MUART_vSendStg(u8 * A_u8PtrData );


#endif /* MCAL_UART_UART_INT_H_ */
