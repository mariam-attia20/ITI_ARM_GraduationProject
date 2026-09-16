/*
 * TFT_prg.c
 *
 *  Created on: Sep 3, 2026
 *      Author: lenovo
 */


#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"


#include "TFT_int.h"

#include "../../MCAL/GPIO/GPIO_int.h"
#include "../../MCAL/SPI/SPI_int.h"
#include "../../MCAL/SYSTICK/SYSTICK_int.h"

static u16 CurrentXStart;
static u16 CurrentXEnd;
static u16 CurrentYStart;
static u16 CurrentYEnd;

GPIOx_PinConfig_t TFT_RST_PIN={
		.Port = GPIO_PORTA,
		.Pin = GPIO_PIN0,
		.Mode = GPIO_MODE_OUTPUT,
		.Speed = GPIO_SPEED_HIGH,
		.OutputType = GPIO_OT_PUSHPULL
};

GPIOx_PinConfig_t TFT_A0_PIN={
		.Port = GPIO_PORTA,
		.Pin = GPIO_PIN1,
		.Mode = GPIO_MODE_OUTPUT,
		.Speed = GPIO_SPEED_HIGH,
		.OutputType = GPIO_OT_PUSHPULL
};



static void Reset_Seq(void)
{
	//RST PIN = 1
	MGPIO_vSetPinValue(TFT_RST_PIN.Port,TFT_RST_PIN.Pin,GPIO_HIGH);
	//DELAY
	MYSYTICK_vSetDelay_us(100);
	//RST PIN 0
	MGPIO_vSetPinValue(TFT_RST_PIN.Port,TFT_RST_PIN.Pin,GPIO_LOW);
	//DELAY 1 MICRO
	MYSYTICK_vSetDelay_us(1);
	//PIN HIGH
	MGPIO_vSetPinValue(TFT_RST_PIN.Port,TFT_RST_PIN.Pin,GPIO_HIGH);
	//DEL 100 MICRO
	MYSYTICK_vSetDelay_us(100);
	//PIN LOW
	MGPIO_vSetPinValue(TFT_RST_PIN.Port,TFT_RST_PIN.Pin,GPIO_LOW);
	//DEL 100 MIC
	MYSYTICK_vSetDelay_us(100);
	//PIN HIGH
	MGPIO_vSetPinValue(TFT_RST_PIN.Port,TFT_RST_PIN.Pin,GPIO_HIGH);
	//DEL 120 MILLI
	MYSYTICK_vSetDelay_ms(120);


}


static void Write_cmd(u8 A_u8cmd)
{
// 0-> cmd
	//1-> data
	MGPIO_vSetPinValue(TFT_A0_PIN.Port,TFT_A0_PIN.Pin,GPIO_LOW);
	MSPI_u8Transceive(A_u8cmd);
}


static void Write_data(u8 A_u8data)
{
	MGPIO_vSetPinValue(TFT_A0_PIN.Port,TFT_A0_PIN.Pin,GPIO_HIGH);
	MSPI_u8Transceive(A_u8data);
}


void HTFT_vInit(void)
{
	//PIN CONFIG
	MGPIO_vPinInit(&TFT_A0_PIN);
	MGPIO_vPinInit(&TFT_RST_PIN);

	//SPI
	MSPI_vInit();

	//SYSTICK
	MYSYSTICK_CFG_T STK_CFG = {
			.InterruptEnable = INT_DISABLE,
			.CLK_SRC = CLK_SRC_AHB_8
	};
	MSYSTICK_vInit(&STK_CFG);
	//RST
	Reset_Seq();
	//SLEEP OUT (CMD)
	Write_cmd(0x11);
	//WAIT 15 msec
	MYSYTICK_vSetDelay_ms(15);
	//Select Mode 3A
	Write_cmd(0x3A);
	//Mode is 565->0x05
	Write_data(0x05);

	//DISPLAY_ON
	Write_cmd(0x29);
}




void HTFT_vShowImage(const u16 A_u16ImgArr[] , u16 A_u16ImgSize)
{
	u8 MSB=0;
	u8 LSB=0;

	//set rows position (colm addres set)
	Write_cmd(0x2A);

	//1 send x start
	//msb
	Write_data(0);
	//least
	Write_data(0);

	// send x End
	// 127 -> 0x7f
	Write_data(0);
	Write_data(127);


	//set colms position (row addres set)
	Write_cmd(0x2B);
	//send y start
	Write_data(0);
	Write_data(0);

	// send y end
	Write_data(0);
	Write_data(159);


	//send image
	Write_cmd(0x2C);

	//0x1234
	//Write_data(12);
	//Write_data(34);


	for(u16 i=0 ; i< A_u16ImgSize ; i++)
	{
		MSB = (A_u16ImgArr[i] & 0xFF00 ) >> 8;
		LSB = (A_u16ImgArr[i] & 0x00FF );

		Write_data(MSB);
		Write_data(LSB);
	}

}
/*
void HTFT_vSetXPos(u16 A_u16xStart, u16 A_u16xEnd)
{
	u8 MSBsart=0;
	u8 LSBstart=0;
	u8 MSBend=0;
	u8 LSBend=0;

	Write_cmd(0x2A);

	MSBsart = (A_u16xStart & 0xFF00 ) >> 8;
	LSBstart = (A_u16xStart & 0x00FF );

	Write_data(MSBsart);
	Write_data(LSBstart);

	MSBend = (A_u16xEnd & 0xFF00 ) >> 8;
	LSBend = (A_u16xEnd & 0x00FF );

	Write_data(MSBend);
	Write_data(LSBend);
}


void HTFT_vSetYPos(u16 A_u16yStart, u16 A_u16yEnd)
{
	u8 MSBsart=0;
	u8 LSBstart=0;
	u8 MSBend=0;
	u8 LSBend=0;

	Write_cmd(0x2B);

	MSBsart = (A_u16yStart & 0xFF00 ) >> 8;
	LSBstart = (A_u16yStart & 0x00FF );

	Write_data(MSBsart);
	Write_data(LSBstart);

	MSBend = (A_u16yEnd & 0xFF00 ) >> 8;
	LSBend = (A_u16yEnd & 0x00FF );

	Write_data(MSBend);
	Write_data(LSBend);

}



void HTFT_vFillBackgroundColor(u16 A_u16Color)
{
	u8 MSB =0;
	u8 LSB =0;
	Write_cmd(0x2C);
	u16 A_u16ImgArr[20480] = {0};
			switch (A_u16Color)
			{
			case Black:
				for(u16 i=0 ; i< 20480 ; i++)
				{
					MSB = (A_u16ImgArr[i] & 0x0000 ) >> 8;
					LSB = (A_u16ImgArr[i] & 0x0000 );

					Write_data(MSB);
					Write_data(LSB);
				}
				break;
			case White :
				for(u16 i=0 ; i< 20480 ; i++)
				{
					MSB = (A_u16ImgArr[i] & 0xFF00 ) >> 8;
					LSB = (A_u16ImgArr[i] & 0x00FF );

					Write_data(MSB);
					Write_data(LSB);
				}
				break;
			}

}

*/

void HTFT_vSetXPos(u16 A_u16xStart, u16 A_u16xEnd)
{
	CurrentXStart= A_u16xStart;
	CurrentXEnd= A_u16xEnd;
	Write_cmd(0x2A);

	//1 send x start
	//msb
	Write_data(0);
	//least
	Write_data(A_u16xStart);

	// send x End
	// 127 -> 0x7f
	Write_data(0);
	Write_data(A_u16xEnd);



}


void HTFT_vSetYPos(u16 A_u16yStart, u16 A_u16yEnd)
{
	CurrentYStart= A_u16yStart;
	CurrentYEnd= A_u16yEnd;
	//set colms position (row addres set)
	Write_cmd(0x2B);
	//send y start
	Write_data(0);
	Write_data(A_u16yStart);

	// send y end
	Write_data(0);
	Write_data(A_u16yEnd);

}



void HTFT_vFillBackgroundColor(u16 A_u16Color)
{
	HTFT_vSetXPos(0,127);
	HTFT_vSetYPos(0,159);
	HTFT_vFillRectangle(A_u16Color);


}
void HTFT_vFillRectangle(u16 A_u16Color)
{
	u8 MSB = 0;
	u8 LSB = 0;
	u16 pixles = (CurrentXEnd-CurrentXStart)*(CurrentYEnd-CurrentYStart);
	Write_cmd(0x2C);


	for(u16 i=0 ; i< pixles ; i++)
	{
		MSB = (A_u16Color & 0xFF00 ) >> 8;
		LSB = (A_u16Color & 0x00FF );

		Write_data(MSB);
		Write_data(LSB);
	}

}

