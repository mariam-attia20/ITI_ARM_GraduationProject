/*
 * UART_prv.h
 *
 *  Created on: 31 Aug 2026
 *      Author: LOQ
 */

#ifndef MCAL_UART_UART_PRV_H_
#define MCAL_UART_UART_PRV_H_

#define USART1_BASE_ADDRESS    0x40013800
#define USART2_BASE_ADDRESS    0x40004400


typedef struct{
	u32 SR;
	u32 DR;
	u32 BRR;
	u32 CR1;
	u32 CR2;
	u32 CR3;
	u32 GTPR;

}UARTx_MemMap_t;

#define USART1 ((volatile UARTx_MemMap_t*)(USART1_BASE_ADDRESS))
#define USART2 ((volatile UARTx_MemMap_t*)(USART2_BASE_ADDRESS))



#endif /* MCAL_UART_UART_PRV_H_ */
