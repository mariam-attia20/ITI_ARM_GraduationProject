/*
 * RC522_prg.c
 *
 *  Created on: Sep 15, 2026
 *      Author: lenovo
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"

#include "../../MCAL/GPIO/GPIO_int.h"
#include "../../MCAL/SPI/SPI_int.h"
#include "../../MCAL/SYSTICK/SYSTICK_int.h"

#include "RC522_int.h"
#include "RC522_prv.h"
#include "RC522_cfg.h"

/* -------------------------------------------------------------------- */
/* CS is fully software controlled (same pattern as the TFT HAL driver) */
static void CS_Low(void)
{
	MGPIO_vSetPinValue(RC522_CS_PORT, RC522_CS_PIN, GPIO_LOW);
}

static void CS_High(void)
{
	MGPIO_vSetPinValue(RC522_CS_PORT, RC522_CS_PIN, GPIO_HIGH);
}

/* -------------------------------------------------------------------- */
/* Low level register access (MFRC522 SPI protocol, datasheet 8.1.2) */
static void WriteReg(u8 A_u8Addr, u8 A_u8Val)
{
	CS_Low();
	MSPI_u8Transceive((A_u8Addr << 1) & 0x7E);	/* bit7 = 0 -> write */
	MSPI_u8Transceive(A_u8Val);
	CS_High();
}

static u8 ReadReg(u8 A_u8Addr)
{
	u8 L_u8Val;

	CS_Low();
	MSPI_u8Transceive(((A_u8Addr << 1) & 0x7E) | 0x80); /* bit7 = 1 -> read */
	L_u8Val = MSPI_u8Transceive(0x00);
	CS_High();

	return L_u8Val;
}

static void SetBitMask(u8 A_u8Addr, u8 A_u8Mask)
{
	WriteReg(A_u8Addr, ReadReg(A_u8Addr) | A_u8Mask);
}

static void ClearBitMask(u8 A_u8Addr, u8 A_u8Mask)
{
	WriteReg(A_u8Addr, ReadReg(A_u8Addr) & (~A_u8Mask));
}

static void AntennaOn(void)
{
	u8 L_u8Temp = ReadReg(REG_TX_CONTROL);
	if (!(L_u8Temp & 0x03))
	{
		SetBitMask(REG_TX_CONTROL, 0x03);
	}
}

/* -------------------------------------------------------------------- */
/* Generic "talk to the card" routine, used by Request() and Anticoll().
 * Sends A_u8SendLen bytes from A_pu8SendData, waits for the command to
 * finish, then copies whatever came back into A_pu8BackData.
 * A_pu16BackLen returns the number of valid bits received. */
static u8 ToCard(u8 A_u8Command, u8* A_pu8SendData, u8 A_u8SendLen,
				  u8* A_pu8BackData, u16* A_pu16BackLen)
{
	u8  L_u8Status   = RC522_ERR;
	u8  L_u8IrqEn    = 0x00;
	u8  L_u8WaitIRq  = 0x00;
	u8  L_u8LastBits;
	u8  L_u8N;
	u16 i;

	if (A_u8Command == PCD_AUTHENT)
	{
		L_u8IrqEn   = 0x12;
		L_u8WaitIRq = 0x10;
	}
	else if (A_u8Command == PCD_TRANSCEIVE)
	{
		L_u8IrqEn   = 0x77;
		L_u8WaitIRq = 0x30;
	}

	WriteReg(REG_COM_IEN, L_u8IrqEn | 0x80);
	ClearBitMask(REG_COM_IRQ, 0x80);
	SetBitMask(REG_FIFO_LEVEL, 0x80);		/* flush FIFO */

	WriteReg(REG_COMMAND, PCD_IDLE);

	for (i = 0; i < A_u8SendLen; i++)
	{
		WriteReg(REG_FIFO_DATA, A_pu8SendData[i]);
	}

	WriteReg(REG_COMMAND, A_u8Command);

	if (A_u8Command == PCD_TRANSCEIVE)
	{
		SetBitMask(REG_BIT_FRAMING, 0x80);	/* StartSend = 1 */
	}

	/* wait for the command to finish (timer based timeout) */
	i = 2000;
	do
	{
		L_u8N = ReadReg(REG_COM_IRQ);
		i--;
	} while ((i != 0) && !(L_u8N & 0x01) && !(L_u8N & L_u8WaitIRq));

	ClearBitMask(REG_BIT_FRAMING, 0x80);

	if (i == 0)
	{
		return RC522_ERR;					/* timeout */
	}

	if (ReadReg(REG_ERROR) & 0x1B)			/* BufferOvfl / CollErr / CRCErr / ProtocolErr */
	{
		return RC522_ERR;
	}

	L_u8Status = RC522_OK;

	if (L_u8N & L_u8IrqEn & 0x01)
	{
		L_u8Status = RC522_NOTAGERR;
	}

	if (A_u8Command == PCD_TRANSCEIVE)
	{
		L_u8N = ReadReg(REG_FIFO_LEVEL);
		L_u8LastBits = ReadReg(REG_CONTROL) & 0x07;

		if (L_u8LastBits)
		{
			*A_pu16BackLen = (L_u8N - 1) * 8 + L_u8LastBits;
		}
		else
		{
			*A_pu16BackLen = L_u8N * 8;
		}

		if (L_u8N == 0)
		{
			L_u8N = 1;
		}
		if (L_u8N > 16)
		{
			L_u8N = 16;
		}

		for (i = 0; i < L_u8N; i++)
		{
			A_pu8BackData[i] = ReadReg(REG_FIFO_DATA);
		}
	}

	return L_u8Status;
}

/* -------------------------------------------------------------------- */
void HRC522_vReset(void)
{
	MGPIO_vSetPinValue(RC522_RST_PORT, RC522_RST_PIN, GPIO_LOW);
	MYSYTICK_vSetDelay_ms(2);
	MGPIO_vSetPinValue(RC522_RST_PORT, RC522_RST_PIN, GPIO_HIGH);
	MYSYTICK_vSetDelay_ms(50);

	WriteReg(REG_COMMAND, PCD_RESETPHASE);
	MYSYTICK_vSetDelay_ms(50);
}

/* Chip-side (register) re-init, shared by HRC522_vInit() and
 * HRC522_vSoftReset(). Does NOT touch GPIO configuration. */
static void ChipReInit(void)
{
	HRC522_vReset();

	/* Timer: TAuto = 1 (timer starts automatically after transmission) */
	WriteReg(REG_T_MODE, 0x8D);
	WriteReg(REG_T_PRESCALER, 0x3E);
	WriteReg(REG_T_RELOAD_L, 30);
	WriteReg(REG_T_RELOAD_H, 0);

	WriteReg(REG_TX_AUTO, 0x40);	/* 100%ASK */
	WriteReg(REG_MODE, 0x3D);		/* CRC preset value 0x6363 */

	AntennaOn();
}

void HRC522_vInit(void)
{
	/* ---- CS pin : GPIO output, idle HIGH ---- */
	GPIOx_PinConfig_t L_xCsPin =
	{
			.Port = RC522_CS_PORT,
			.Pin = RC522_CS_PIN,
			.Mode = GPIO_MODE_OUTPUT,
			.Speed = GPIO_SPEED_HIGH,
			.OutputType = GPIO_OT_PUSHPULL,
			.PullType = GPIO_NO_PULL
	};
	MGPIO_vPinInit(&L_xCsPin);
	CS_High();

	/* ---- RST pin : GPIO output ---- */
	GPIOx_PinConfig_t L_xRstPin =
	{
			.Port = RC522_RST_PORT,
			.Pin = RC522_RST_PIN,
			.Mode = GPIO_MODE_OUTPUT,
			.Speed = GPIO_SPEED_HIGH,
			.OutputType = GPIO_OT_PUSHPULL,
			.PullType = GPIO_NO_PULL
	};
	MGPIO_vPinInit(&L_xRstPin);
	MGPIO_vSetPinValue(RC522_RST_PORT, RC522_RST_PIN, GPIO_HIGH);

	/* NOTE: SCK/MOSI/MISO (PA5/PA7/PA6) and the SPI1 peripheral itself
	 * are configured by the application (same convention as the TFT
	 * driver in this project) before calling HRC522_vInit(). */

	ChipReInit();
}

void HRC522_vSoftReset(void)
{
	ChipReInit();
}

/* -------------------------------------------------------------------- */
static u8 Request(u8 A_u8ReqMode, u8* A_pu8TagType)
{
	u8  L_u8Status;
	u16 L_u16BackBits;

	WriteReg(REG_BIT_FRAMING, 0x07);	/* TxLastBits = 7 (short frame) */

	A_pu8TagType[0] = A_u8ReqMode;
	L_u8Status = ToCard(PCD_TRANSCEIVE, A_pu8TagType, 1, A_pu8TagType, &L_u16BackBits);

	if ((L_u8Status != RC522_OK) || (L_u16BackBits != 0x10))
	{
		L_u8Status = RC522_ERR;
	}

	return L_u8Status;
}

static u8 Anticoll(u8* A_pu8SerNum)
{
	u8  L_u8Status;
	u8  i;
	u8  L_u8CheckSum = 0;
	u16 L_u16UnLen;

	WriteReg(REG_BIT_FRAMING, 0x00);	/* TxLastBits = 0 (full byte frame) */

	A_pu8SerNum[0] = PICC_ANTICOLL;
	A_pu8SerNum[1] = 0x20;
	L_u8Status = ToCard(PCD_TRANSCEIVE, A_pu8SerNum, 2, A_pu8SerNum, &L_u16UnLen);

	if (L_u8Status == RC522_OK)
	{
		for (i = 0; i < 4; i++)
		{
			L_u8CheckSum ^= A_pu8SerNum[i];
		}
		if (L_u8CheckSum != A_pu8SerNum[4])
		{
			L_u8Status = RC522_ERR;
		}
	}

	return L_u8Status;
}

/* -------------------------------------------------------------------- */
u8 HRC522_u8IsCardPresent(void)
{
	u8 L_u8TagType[2];
	return Request(PICC_REQIDL, L_u8TagType);
}

u8 HRC522_u8ReadCardUID(u8* A_pu8UID)
{
	u8 L_u8TagType[2];

	if (Request(PICC_REQIDL, L_u8TagType) != RC522_OK)
	{
		return RC522_ERR;
	}

	return Anticoll(A_pu8UID);
}
