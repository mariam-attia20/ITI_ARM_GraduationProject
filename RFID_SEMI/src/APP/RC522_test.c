/*
 * RC522_test.c
 *
 *  Created on: Sep 15, 2026
 *      Author: lenovo
 *
 * MINIMAL WIRING TEST - does NOT use the RC522 HAL driver on purpose,
 * so a bug in that driver can't hide a wiring/power problem or vice versa.
 * It just reads the chip's VersionReg (0x37) over raw SPI and reports it:
 *
 *   - Every ~500ms it toggles PA0 (heartbeat -> proves the code is running
 *     and flashed correctly, independent of the RC522).
 *   - It also prints the version byte as two hex ASCII chars + newline on
 *     USART1 (PA9 = TX) at 9600 baud, e.g. "91" or "92" for a genuine
 *     MFRC522, or "00"/"FF" if SPI communication is NOT working.
 *
 * AL TAWSELA (wiring) :-
 * RC522 VCC  -> 3.3V   (NOT 5V - this alone kills a lot of boards/reads)
 * RC522 GND  -> GND    (common ground with the STM32 + your USB-UART)
 * RC522 SCK  -> PA5
 * RC522 MOSI -> PA7
 * RC522 MISO -> PA6
 * RC522 SDA  -> PA4  (CS / SS)
 * RC522 RST  -> PA3
 * USB-UART RX -> PA9 (STM32 TX)   [only needed if you want the printed value]
 *
 * Remember: only ONE main() can be active in the whole project at build
 * time. Comment out any other active main() (e.g. in TWEL_SEPT.c, or
 * RFID_APP.c if you already added it) before building this test.
 */

#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

#include "../MCAL/RCC/RCC_int.h"
#include "../MCAL/GPIO/GPIO_int.h"
#include "../MCAL/SPI/SPI_int.h"
#include "../MCAL/UART/UART_int.h"
#include "../MCAL/SYSTICK/SYSTICK_int.h"

#define CS_PORT		GPIO_PORTA
#define CS_PIN		GPIO_PIN4
#define RST_PORT	GPIO_PORTA
#define RST_PIN		GPIO_PIN3
#define LED_PORT	GPIO_PORTA
#define LED_PIN		GPIO_PIN0

static void CS_Low(void)  { MGPIO_vSetPinValue(CS_PORT, CS_PIN, GPIO_LOW); }
static void CS_High(void) { MGPIO_vSetPinValue(CS_PORT, CS_PIN, GPIO_HIGH); }

static u8 ReadRegRaw(u8 A_u8Addr)
{
	u8 L_u8Val;
	CS_Low();
	MSPI_u8Transceive(((A_u8Addr << 1) & 0x7E) | 0x80);
	L_u8Val = MSPI_u8Transceive(0x00);
	CS_High();
	return L_u8Val;
}

static void SendHexByte(u8 A_u8Val)
{
	/* NOTE: MUART_vSendChar() in this project's UART driver is interrupt
	 * driven and has a bug - its ISR reads a different global pointer
	 * (G_u8PtrData) than the one MUART_vSendChar() actually sets
	 * (G_u8TxData), so G_u8PtrData stays NULL and the ISR crashes/hangs
	 * on a NULL dereference the first time the interrupt fires. That is
	 * almost certainly why the loop froze after lighting the LED once.
	 * MUART_vSendData() below is the fully blocking / polling version in
	 * the same driver and does not touch those broken globals at all. */
	const char L_cHex[] = "0123456789ABCDEF";
	MUART_vSendData(L_cHex[(A_u8Val >> 4) & 0x0F]);
	MUART_vSendData(L_cHex[A_u8Val & 0x0F]);
	MUART_vSendData('\r');
	MUART_vSendData('\n');
}

int main(void)
{
	MRCC_vInit();
	MRCC_vEnableClk(RCC_APB2, RCC_GPIOA);	/* GPIOA clock */
	MRCC_vEnableClk(RCC_APB2, 12);			/* SPI1 clock  (bit12) */
	MRCC_vEnableClk(RCC_APB2, 14);			/* USART1 clock (bit14) */

	MYSYSTICK_CFG_T L_xStkCfg =
	{
			.InterruptEnable = INT_DISABLE,
			.CLK_SRC = CLK_SRC_AHB_8
	};
	MSYSTICK_vInit(&L_xStkCfg);

	/* SPI1 pins */
	GPIOx_PinConfig_t L_xSCK =
	{ .Port=GPIO_PORTA, .Pin=GPIO_PIN5, .Mode=GPIO_MODE_ALF,
	  .Speed=GPIO_SPEED_HIGH, .OutputType=GPIO_OT_PUSHPULL, .PullType=GPIO_NO_PULL };
	MGPIO_vPinInit(&L_xSCK);

	GPIOx_PinConfig_t L_xMOSI =
	{ .Port=GPIO_PORTA, .Pin=GPIO_PIN7, .Mode=GPIO_MODE_ALF,
	  .Speed=GPIO_SPEED_HIGH, .OutputType=GPIO_OT_PUSHPULL, .PullType=GPIO_NO_PULL };
	MGPIO_vPinInit(&L_xMOSI);

	GPIOx_PinConfig_t L_xMISO =
	{ .Port=GPIO_PORTA, .Pin=GPIO_PIN6, .Mode=GPIO_MODE_INPUT, .PullType=GPIO_NO_PULL };
	MGPIO_vPinInit(&L_xMISO);

	MSPI_vInit();

	/* CS / RST */
	GPIOx_PinConfig_t L_xCS =
	{ .Port=CS_PORT, .Pin=CS_PIN, .Mode=GPIO_MODE_OUTPUT,
	  .Speed=GPIO_SPEED_HIGH, .OutputType=GPIO_OT_PUSHPULL, .PullType=GPIO_NO_PULL };
	MGPIO_vPinInit(&L_xCS);
	CS_High();

	GPIOx_PinConfig_t L_xRST =
	{ .Port=RST_PORT, .Pin=RST_PIN, .Mode=GPIO_MODE_OUTPUT,
	  .Speed=GPIO_SPEED_HIGH, .OutputType=GPIO_OT_PUSHPULL, .PullType=GPIO_NO_PULL };
	MGPIO_vPinInit(&L_xRST);

	/* hardware reset pulse */
	MGPIO_vSetPinValue(RST_PORT, RST_PIN, GPIO_LOW);
	MYSYTICK_vSetDelay_ms(2);
	MGPIO_vSetPinValue(RST_PORT, RST_PIN, GPIO_HIGH);
	MYSYTICK_vSetDelay_ms(50);

	/* LED (heartbeat) */
	GPIOx_PinConfig_t L_xLed =
	{ .Port=LED_PORT, .Pin=LED_PIN, .Mode=GPIO_MODE_OUTPUT,
	  .Speed=GPIO_SPEED_LOW, .OutputType=GPIO_OT_PUSHPULL, .PullType=GPIO_NO_PULL };
	MGPIO_vPinInit(&L_xLed);

	/* UART (debug print) */
	MUART_vInt();
	GPIOx_PinConfig_t L_xTX =
	{ .Port=GPIO_PORTA, .Pin=GPIO_PIN9, .Mode=GPIO_MODE_ALF,
	  .Speed=GPIO_SPEED_HIGH, .OutputType=GPIO_OT_PUSHPULL };
	MGPIO_vPinInit(&L_xTX);

	while (1)
	{
		u8 L_u8Version = ReadRegRaw(0x37);	/* VersionReg */

		SendHexByte(L_u8Version);

		MGPIO_TogPinValue(LED_PORT, LED_PIN);	/* heartbeat blink */
		MYSYTICK_vSetDelay_ms(500);
	}
}
