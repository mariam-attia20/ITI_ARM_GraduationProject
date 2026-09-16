///*
// * RC522_UID_test.c
// *
// *  Created on: Sep 15, 2026
// *      Author: lenovo
// *
// * Scan each of your cards/tags one at a time with this program running,
// * and note down the 4-byte UID it prints for each one. You'll paste those
// * UIDs into the KnownCards[] table in RFID_APP.c afterwards.
// *
// * Output (USART1, PA9 = TX, 9600 baud), one line per detected card, e.g.:
// *   UID: DE AD BE EF
// *
// * Wiring: same as RFID_APP.c (RC522 on SPI1 PA5/PA6/PA7, CS=PA4, RST=PA3),
// * plus a USB-UART adapter's RX pin on PA9.
// *
// * Only one main() can be active in the build - exclude/comment out any
// * other main() (RFID_APP.c, RC522_test.c, etc.) before building this.
// */
//
//#include "../LIB/STD_TYPES.h"
//#include "../LIB/BIT_MATH.h"
//
//#include "../MCAL/RCC/RCC_int.h"
//#include "../MCAL/GPIO/GPIO_int.h"
//#include "../MCAL/SPI/SPI_int.h"
//#include "../MCAL/UART/UART_int.h"
//#include "../MCAL/SYSTICK/SYSTICK_int.h"
//
//#include "../HAL/RC522/RC522_int.h"
//
//static void SendHexByte(u8 A_u8Val)
//{
//	/* MUART_vSendData() is the blocking/polling send - MUART_vSendChar()
//	 * in this project's UART driver is interrupt-driven and buggy (see
//	 * RC522_test.c notes), so it's avoided here on purpose. */
//	const char L_cHex[] = "0123456789ABCDEF";
//	MUART_vSendData(L_cHex[(A_u8Val >> 4) & 0x0F]);
//	MUART_vSendData(L_cHex[A_u8Val & 0x0F]);
//}
//
//int main(void)
//{
//	MRCC_vInit();
//	MRCC_vEnableClk(RCC_APB2, RCC_GPIOA);
//	MRCC_vEnableClk(RCC_APB2, 12);	/* SPI1  */
//	MRCC_vEnableClk(RCC_APB2, 14);	/* USART1 */
//
//	MYSYSTICK_CFG_T L_xStkCfg =
//	{ .InterruptEnable = INT_DISABLE, .CLK_SRC = CLK_SRC_AHB_8 };
//	MSYSTICK_vInit(&L_xStkCfg);
//
//	GPIOx_PinConfig_t L_xSCK =
//	{ .Port=GPIO_PORTA, .Pin=GPIO_PIN5, .Mode=GPIO_MODE_ALF,
//	  .Speed=GPIO_SPEED_HIGH, .OutputType=GPIO_OT_PUSHPULL, .PullType=GPIO_NO_PULL };
//	MGPIO_vPinInit(&L_xSCK);
//
//	GPIOx_PinConfig_t L_xMOSI =
//	{ .Port=GPIO_PORTA, .Pin=GPIO_PIN7, .Mode=GPIO_MODE_ALF,
//	  .Speed=GPIO_SPEED_HIGH, .OutputType=GPIO_OT_PUSHPULL, .PullType=GPIO_NO_PULL };
//	MGPIO_vPinInit(&L_xMOSI);
//
//	GPIOx_PinConfig_t L_xMISO =
//	{ .Port=GPIO_PORTA, .Pin=GPIO_PIN6, .Mode=GPIO_MODE_INPUT, .PullType=GPIO_NO_PULL };
//	MGPIO_vPinInit(&L_xMISO);
//
//	MSPI_vInit();
//
//	MUART_vInt();
//	GPIOx_PinConfig_t L_xTX =
//	{ .Port=GPIO_PORTA, .Pin=GPIO_PIN9, .Mode=GPIO_MODE_ALF,
//	  .Speed=GPIO_SPEED_HIGH, .OutputType=GPIO_OT_PUSHPULL };
//	MGPIO_vPinInit(&L_xTX);
//
//	HRC522_vInit();
//
//	while (1)
//	{
//		u8 L_u8UID[RC522_UID_LEN];
//
//		HRC522_vSoftReset();	/* stay reliable across repeated scans */
//
//		if (HRC522_u8ReadCardUID(L_u8UID) == RC522_OK)
//		{
//			MUART_vSendData('U'); MUART_vSendData('I'); MUART_vSendData('D');
//			MUART_vSendData(':'); MUART_vSendData(' ');
//
//			for (u8 i = 0; i < 4; i++)	/* only the 4 UID bytes, skip the BCC checksum byte */
//			{
//				SendHexByte(L_u8UID[i]);
//				MUART_vSendData(' ');
//			}
//			MUART_vSendData('\r');
//			MUART_vSendData('\n');
//		}
//
//		MYSYTICK_vSetDelay_ms(300);
//	}
//}
