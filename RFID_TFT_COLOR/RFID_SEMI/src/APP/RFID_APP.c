///*
// * RFID_APP.c
// *
// *  Created on: Sep 15, 2026
// *      Author: lenovo
// *
// * Multi-card version: reads each card's UID and fills the TFT screen with
// * a DIFFERENT color depending on WHICH of your 4 cards was scanned.
// * Unknown cards / no card -> screen goes black.
// *
// *   Card 1 (9D 41 17 EF) -> RED
// *   Card 2 (88 04 73 BE) -> BLUE
// *   Card 3 (DD BB FD EE) -> GREEN
// *   Card 4 (E5 67 23 03) -> YELLOW
// *
// * AL TAWSELA (wiring) - RC522 and TFT share SPI1 (SCK/MOSI/MISO), each
// * with its own CS pin so they don't interfere with each other :-
// *
// * RC522 VCC   -> 3.3V   (NOT 5V)
// * RC522 GND   -> GND
// * RC522 SCK   -> PA5   (shared SPI1 SCK)
// * RC522 MOSI  -> PA7   (shared SPI1 MOSI)
// * RC522 MISO  -> PA6   (shared SPI1 MISO - TFT does not use this line)
// * RC522 SDA   -> PA4   (RC522's own CS / SS)
// * RC522 RST   -> PA3   (RC522's own reset)
// *
// * TFT   VCC   -> 3.3V
// * TFT   GND   -> GND
// * TFT   SCK   -> PA5   (shared SPI1 SCK, same wire as RC522's)
// * TFT   SDA/MOSI -> PA7 (shared SPI1 MOSI, same wire as RC522's)
// * TFT   RST   -> PA0   (TFT's own reset)
// * TFT   A0/DC -> PA1   (TFT's own command/data select)
// * TFT   CS    -> PA2   (TFT's own CS - do NOT tie this to GND anymore,
// *                        it must go to PA2 so it doesn't listen in on
// *                        traffic meant for the RC522)
// * TFT   LED/BL-> 3.3V (or a GPIO if your module has a controllable
// *                        backlight pin; most run it straight to 3.3V)
// *
// * NOTE: only ONE main() can be active in the build at a time - exclude
// * (or comment out) every other file with an active main() before building.
// */
//
//#include "../LIB/STD_TYPES.h"
//#include "../LIB/BIT_MATH.h"
//
//#include "../MCAL/RCC/RCC_int.h"
//#include "../MCAL/GPIO/GPIO_int.h"
//#include "../MCAL/SPI/SPI_int.h"
//#include "../MCAL/SYSTICK/SYSTICK_int.h"
//
//#include "../HAL/RC522/RC522_int.h"
//#include "../HAL/TFT/TFT_int.h"
//
///* ---------------------------------------------------------------------
// * KNOWN CARDS TABLE - each card's UID + which color it should show.
// * ------------------------------------------------------------------- */
//typedef struct
//{
//	u8  UID[4];
//	u16 Color;
//} CardEntry_t;
//
//static const CardEntry_t KnownCards[] =
//{
//	{ {0x9D, 0x41, 0x17, 0xEF}, TFT_COLOR_RED },		/* Card 1 -> RED */
//	{ {0x88, 0x04, 0x73, 0xBE}, TFT_COLOR_BLUE },		/* Card 2 -> BLUE */
//	{ {0xDD, 0xBB, 0xFD, 0xEE}, TFT_COLOR_GREEN },		/* Card 3 -> GREEN */
//	{ {0xE5, 0x67, 0x23, 0x03}, TFT_COLOR_YELLOW },	/* Card 4 -> YELLOW */
//};
//
//#define NUM_KNOWN_CARDS		(sizeof(KnownCards) / sizeof(KnownCards[0]))
//
///* -------------------------------------------------------------------- */
//static u8 UidMatches(const u8* A_pu8UID, const u8* A_pu8Known)
//{
//	u8 i;
//	for (i = 0; i < 4; i++)
//	{
//		if (A_pu8UID[i] != A_pu8Known[i])
//		{
//			return 0;
//		}
//	}
//	return 1;
//}
//
///* Returns the index into KnownCards[] for a matching UID, or -1 if the
// * card isn't in the table. */
//static s8 FindCard(const u8* A_pu8UID)
//{
//	u8 i;
//	for (i = 0; i < NUM_KNOWN_CARDS; i++)
//	{
//		if (UidMatches(A_pu8UID, KnownCards[i].UID))
//		{
//			return (s8)i;
//		}
//	}
//	return -1;
//}
//
//int main(void)
//{
//	MRCC_vInit();
//	MRCC_vEnableClk(RCC_APB2, RCC_GPIOA);	/* GPIOA clock (RC522 + TFT + SPI1 pins) */
//	MRCC_vEnableClk(RCC_APB2, 12);			/* SPI1 clock (APB2ENR bit12) */
//
//	/* ---- SysTick (used for RC522/TFT reset delays) ---- */
//	MYSYSTICK_CFG_T L_xStkCfg =
//	{
//			.InterruptEnable = INT_DISABLE,
//			.CLK_SRC = CLK_SRC_AHB_8
//	};
//	MSYSTICK_vInit(&L_xStkCfg);
//
//	/* ---- SPI1 pins : SCK = PA5, MOSI = PA7 (alternate function push-pull) ---- */
//	GPIOx_PinConfig_t L_xSCK =
//	{
//			.Port = GPIO_PORTA,
//			.Pin = GPIO_PIN5,
//			.Mode = GPIO_MODE_ALF,
//			.Speed = GPIO_SPEED_HIGH,
//			.OutputType = GPIO_OT_PUSHPULL,
//			.PullType = GPIO_NO_PULL
//	};
//	MGPIO_vPinInit(&L_xSCK);
//
//	GPIOx_PinConfig_t L_xMOSI =
//	{
//			.Port = GPIO_PORTA,
//			.Pin = GPIO_PIN7,
//			.Mode = GPIO_MODE_ALF,
//			.Speed = GPIO_SPEED_HIGH,
//			.OutputType = GPIO_OT_PUSHPULL,
//			.PullType = GPIO_NO_PULL
//	};
//	MGPIO_vPinInit(&L_xMOSI);
//
//	/* MISO = PA6 : input floating (master receives, must NOT be alt-function output) */
//	GPIOx_PinConfig_t L_xMISO =
//	{
//			.Port = GPIO_PORTA,
//			.Pin = GPIO_PIN6,
//			.Mode = GPIO_MODE_INPUT,
//			.PullType = GPIO_NO_PULL
//	};
//	MGPIO_vPinInit(&L_xMISO);
//
//	MSPI_vInit();
//
//	/* ---- TFT (configures its own RST/A0/CS pins internally: PA0/PA1/PA2) ---- */
//	HTFT_vInit();
//	HTFT_vFillBackgroundColor(TFT_COLOR_BLACK);	/* start blank */
//
//	/* ---- RC522 (configures its own CS/RST pins internally: PA4/PA3) ---- */
//	HRC522_vInit();
//
//	u8 L_u8UID[RC522_UID_LEN];
//	s8 L_s8LastMatch = -2;	/* -2 = "nothing drawn yet", forces the first draw */
//
//	while (1)
//	{
//		/* Re-init the RC522's internal state before every single poll -
//		 * without this the reader can detect a card once and then get
//		 * stuck until a hardware reset. */
//		HRC522_vSoftReset();
//
//		s8 L_s8Match = -1;	/* -1 = no card / unrecognized card */
//
//		if (HRC522_u8ReadCardUID(L_u8UID) == RC522_OK)
//		{
//			L_s8Match = FindCard(L_u8UID);
//		}
//
//		/* Only touch the screen when the detected card actually changes -
//		 * a full-screen fill takes a noticeable number of SPI bytes, so
//		 * redrawing every single 100ms poll would slow things down and
//		 * isn't needed if nothing changed. */
//		if (L_s8Match != L_s8LastMatch)
//		{
//			if (L_s8Match >= 0)
//			{
//				HTFT_vFillBackgroundColor(KnownCards[L_s8Match].Color);
//			}
//			else
//			{
//				HTFT_vFillBackgroundColor(TFT_COLOR_BLACK);
//			}
//			L_s8LastMatch = L_s8Match;
//		}
//
//		MYSYTICK_vSetDelay_ms(100);
//	}
//}



/*
 * RFID_APP.c
 *
 * Multi-card version:
 * Reads each card's UID and fills the TFT screen with a DIFFERENT color
 * depending on WHICH of your 4 cards was scanned.
 *
 * IMPORTANT BEHAVIOR:
 * -------------------
 * Once a known card is detected, its color stays on the TFT screen.
 *
 * The screen DOES NOT turn black when:
 *   - The card is removed
 *   - No card is detected
 *   - An unknown card is detected
 *
 * The screen changes ONLY when a NEW KNOWN RFID card is detected.
 *
 * Card 1 (9D 41 17 EF) -> RED
 * Card 2 (88 04 73 BE) -> BLUE
 * Card 3 (DD BB FD EE) -> GREEN
 * Card 4 (E5 67 23 03) -> YELLOW
 *
 *
 * AL TAWSELA (wiring)
 * -------------------
 *
 * RC522:
 * VCC  -> 3.3V
 * GND  -> GND
 * SCK  -> PA5
 * MOSI -> PA7
 * MISO -> PA6
 * SDA  -> PA4  (CS / SS)
 * RST  -> PA3
 *
 * TFT:
 * VCC  -> 3.3V
 * GND  -> GND
 * SCK  -> PA5
 * SDA  -> PA7
 * RST  -> PA0
 * A0/DC -> PA1
 * CS    -> PA2
 * LED/BL -> 3.3V
 *
 */



#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

#include "../MCAL/RCC/RCC_int.h"
#include "../MCAL/GPIO/GPIO_int.h"
#include "../MCAL/SPI/SPI_int.h"
#include "../MCAL/SYSTICK/SYSTICK_int.h"

#include "../HAL/RC522/RC522_int.h"
#include "../HAL/TFT/TFT_int.h"


/* ---------------------------------------------------------------------
 * KNOWN CARDS TABLE
 * ------------------------------------------------------------------- */

typedef struct
{
	u8  UID[4];
	u16 Color;

} CardEntry_t;


static const CardEntry_t KnownCards[] =
{
	{ {0x9D, 0x41, 0x17, 0xEF}, TFT_COLOR_RED    },	/* Card 1 */
	{ {0x88, 0x04, 0x73, 0xBE}, TFT_COLOR_BLUE   },	/* Card 2 */
	{ {0xDD, 0xBB, 0xFD, 0xEE}, TFT_COLOR_GREEN  },	/* Card 3 */
	{ {0xE5, 0x67, 0x23, 0x03}, TFT_COLOR_YELLOW },	/* Card 4 */
};


#define NUM_KNOWN_CARDS \
	(sizeof(KnownCards) / sizeof(KnownCards[0]))


/* ---------------------------------------------------------------------
 * Compare RFID UID with known UID
 * ------------------------------------------------------------------- */

static u8 UidMatches(const u8* A_pu8UID,
                     const u8* A_pu8Known)
{
	u8 i;

	for (i = 0; i < 4; i++)
	{
		if (A_pu8UID[i] != A_pu8Known[i])
		{
			return 0;
		}
	}

	return 1;
}


/* ---------------------------------------------------------------------
 * Find the detected card
 *
 * Returns:
 *   0,1,2,3 -> known card index
 *   -1       -> unknown card
 * ------------------------------------------------------------------- */

static s8 FindCard(const u8* A_pu8UID)
{
	u8 i;

	for (i = 0; i < NUM_KNOWN_CARDS; i++)
	{
		if (UidMatches(A_pu8UID, KnownCards[i].UID))
		{
			return (s8)i;
		}
	}

	return -1;
}


/* =====================================================================
 * MAIN
 * =================================================================== */

int main(void)
{
	/* ---------------------------------------------------------------
	 * RCC
	 * ------------------------------------------------------------- */

	MRCC_vInit();

	/* GPIOA clock */
	MRCC_vEnableClk(RCC_APB2, RCC_GPIOA);

	/* SPI1 clock */
	MRCC_vEnableClk(RCC_APB2, 12);


	/* ---------------------------------------------------------------
	 * SysTick
	 * ------------------------------------------------------------- */

	MYSYSTICK_CFG_T L_xStkCfg =
	{
		.InterruptEnable = INT_DISABLE,
		.CLK_SRC = CLK_SRC_AHB_8
	};

	MSYSTICK_vInit(&L_xStkCfg);


	/* ---------------------------------------------------------------
	 * SPI1 PINS
	 *
	 * PA5 -> SCK
	 * PA7 -> MOSI
	 * PA6 -> MISO
	 * ------------------------------------------------------------- */

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


	GPIOx_PinConfig_t L_xMISO =
	{
		.Port = GPIO_PORTA,
		.Pin = GPIO_PIN6,
		.Mode = GPIO_MODE_INPUT,
		.PullType = GPIO_NO_PULL
	};

	MGPIO_vPinInit(&L_xMISO);


	/* Initialize SPI */
	MSPI_vInit();


	/* ---------------------------------------------------------------
	 * TFT INITIALIZATION
	 * ------------------------------------------------------------- */

	HTFT_vInit();

	/* Initial screen = BLACK */
	HTFT_vFillBackgroundColor(TFT_COLOR_BLACK);


	/* ---------------------------------------------------------------
	 * RC522 INITIALIZATION
	 * ------------------------------------------------------------- */

	HRC522_vInit();


	/* ---------------------------------------------------------------
	 * RFID UID BUFFER
	 * ------------------------------------------------------------- */

	u8 L_u8UID[RC522_UID_LEN];


	/*
	 * IMPORTANT:
	 *
	 * This variable stores the LAST KNOWN CARD.
	 *
	 * -1 means that no known card has been detected yet.
	 *
	 * Once a card is detected, its index is stored here.
	 *
	 * We DO NOT reset this variable when the card is removed.
	 *
	 * This is what makes the TFT color HOLD.
	 */

	s8 L_s8CurrentColor = -1;


	/* ---------------------------------------------------------------
	 * MAIN LOOP
	 * ------------------------------------------------------------- */

	while (1)
	{
		/*
		 * Soft reset RC522 before every poll.
		 *
		 * This is kept from your original working code.
		 */

		HRC522_vSoftReset();


		/*
		 * Try to read an RFID card.
		 *
		 * IMPORTANT:
		 *
		 * We do NOT set L_s8CurrentColor to -1 here.
		 *
		 * Therefore, if no card is detected, the previous color
		 * remains on the TFT.
		 */

		if (HRC522_u8ReadCardUID(L_u8UID) == RC522_OK)
		{
			/*
			 * Check whether the detected UID belongs to one
			 * of our four known cards.
			 */

			s8 L_s8DetectedCard = FindCard(L_u8UID);


			/*
			 * Only change the TFT if:
			 *
			 * 1. A known card was detected
			 * 2. It is DIFFERENT from the currently displayed card
			 */

			if ((L_s8DetectedCard >= 0) &&
				(L_s8DetectedCard != L_s8CurrentColor))
			{
				/*
				 * Change TFT color.
				 */

				HTFT_vFillBackgroundColor(
					KnownCards[L_s8DetectedCard].Color
				);


				/*
				 * Save this card as the current card.
				 *
				 * This color will remain even after the card
				 * is removed from the RC522.
				 */

				L_s8CurrentColor = L_s8DetectedCard;
			}
		}


		/*
		 * Small delay before the next RFID scan.
		 */

		MYSYTICK_vSetDelay_ms(100);
	}
}
