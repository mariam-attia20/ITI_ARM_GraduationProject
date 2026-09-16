/*
 * RFID_APP.c
 *
 *  Created on: Sep 15, 2026
 *      Author: lenovo
 *
 * Turns the LED on PA0 ON while an RFID card/tag is in front of the
 * RC522 reader, and OFF otherwise.
 *
 * AL TAWSELA (wiring) :-
 * RC522 VCC  -> 3.3V   (NOT 5V)
 * RC522 GND  -> GND
 * RC522 SCK  -> PA5
 * RC522 MOSI -> PA7
 * RC522 MISO -> PA6
 * RC522 SDA  -> PA4  (CS / SS)
 * RC522 RST  -> PA3
 * LED (+)    -> PA0  (through a resistor, LED(-) -> GND)
 *
 * NOTE: this project currently has TWEL_SEPT.c with an active main().
 * Only one main() can exist in the build, so comment out TWEL_SEPT.c's
 * main() (or exclude that file) before building this lab, the same way
 * TFT.c / TFT2.c are kept commented out when they are not the active lab.
 */

#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

#include "../MCAL/RCC/RCC_int.h"
#include "../MCAL/GPIO/GPIO_int.h"
#include "../MCAL/SPI/SPI_int.h"
#include "../MCAL/SYSTICK/SYSTICK_int.h"

#include "../HAL/RC522/RC522_int.h"

#define LED_PORT	GPIO_PORTA
#define LED_PIN		GPIO_PIN0

int main(void)
{
	MRCC_vInit();
	MRCC_vEnableClk(RCC_APB2, RCC_GPIOA);	/* GPIOA clock */
	MRCC_vEnableClk(RCC_APB2, 12);			/* SPI1 clock (APB2ENR bit12) */

	/* ---- SysTick (used for RC522 reset delays) ---- */
	MYSYSTICK_CFG_T L_xStkCfg =
	{
			.InterruptEnable = INT_DISABLE,
			.CLK_SRC = CLK_SRC_AHB_8
	};
	MSYSTICK_vInit(&L_xStkCfg);

	/* ---- SPI1 pins : SCK = PA5, MOSI = PA7 (alternate function push-pull) ---- */
	GPIOx_PinConfig_t L_xSCK =
	{
			.Port = GPIO_PORTA,
			.Pin = GPIO_PIN5,
			.Mode = GPIO_MODE_ALF,
			.Speed = GPIO_SPEED_HIGH,
			.OutputType = GPIO_OT_PUSHPULL,
			.PullType = GPIO_NO_PULL
	};
	MGPIO_vPinInit(&L_xSCK);

	GPIOx_PinConfig_t L_xMOSI =
	{
			.Port = GPIO_PORTA,
			.Pin = GPIO_PIN7,
			.Mode = GPIO_MODE_ALF,
			.Speed = GPIO_SPEED_HIGH,
			.OutputType = GPIO_OT_PUSHPULL,
			.PullType = GPIO_NO_PULL
	};
	MGPIO_vPinInit(&L_xMOSI);

	/* MISO = PA6 : input floating (master receives, must NOT be alt-function output) */
	GPIOx_PinConfig_t L_xMISO =
	{
			.Port = GPIO_PORTA,
			.Pin = GPIO_PIN6,
			.Mode = GPIO_MODE_INPUT,
			.PullType = GPIO_NO_PULL
	};
	MGPIO_vPinInit(&L_xMISO);

	MSPI_vInit();

	/* ---- LED on PA0 ---- */
	GPIOx_PinConfig_t L_xLed =
	{
			.Port = LED_PORT,
			.Pin = LED_PIN,
			.Mode = GPIO_MODE_OUTPUT,
			.Speed = GPIO_SPEED_LOW,
			.OutputType = GPIO_OT_PUSHPULL,
			.PullType = GPIO_NO_PULL
	};
	MGPIO_vPinInit(&L_xLed);
	MGPIO_vSetPinValue(LED_PORT, LED_PIN, GPIO_LOW);

	/* ---- RC522 (configures its own CS/RST pins internally) ---- */
	HRC522_vInit();

	while (1)
	{
		/* Re-init the chip's internal state (timer/IRQ/error/antenna
		 * registers) before every single poll. Without this, the chip
		 * can end up detecting a card only once and then getting stuck
		 * until a hardware reset - this forces it back to a clean state
		 * every cycle instead, at the cost of a few ms per poll. */
		HRC522_vSoftReset();

		if (HRC522_u8IsCardPresent() == RC522_OK)
		{
			MGPIO_vSetPinValue(LED_PORT, LED_PIN, GPIO_HIGH);
		}
		else
		{
			MGPIO_vSetPinValue(LED_PORT, LED_PIN, GPIO_LOW);
		}

		MYSYTICK_vSetDelay_ms(100);
	}
}
