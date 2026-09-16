/*
 * ESP8266_prg.c
 *
 *  Created on: Sep 8, 2026
 *      Author: lenovo
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"
#include "../../MCAL/GPIO/GPIO_int.h"
#include "../../MCAL/SPI/SPI_int.h"
#include "../../MCAL/SYSTICK/SYSTICK_int.h"
#include "../../MCAL/UART/UART_int.h"


#include "ESP8266_cfg.h"
#include "ESP8266_int.h"



static u16 StringLength(const char*str)
{
	u16 len=0;
	while (str[len]!='\0')
	{
		len++;
	}
	return len;
}

static void inttostr (u16 val,char* outstr)
{
	char temp[6];
	s8 i=0;
	s8 j=0;
	//or s8 i=0 , j=0;
	if(val==0)
	{
		outstr[0]='0';
		outstr[1]='\0';
		return;
	}
	//12
	while(val>0)
	{
		temp[i++]=(val%10)+'0';
		val/=10;
	}
	for(j=0 ; j<i ; j++)
	{
		outstr[j]=temp[i-1-j];
	}
	outstr[j]='\0';
}


void HESP8266_vInit()
{
	MYSYSTICK_CFG_T STK_CFG = {
			.InterruptEnable = INT_DISABLE,
			.CLK_SRC = CLK_SRC_AHB_8
	};
	MSYSTICK_vInit(&STK_CFG);

	MUART_vInt();

	//DISABLE ECHO
	MUART_vSendStg((u8*)"ATE0\r\n");
	MYSYTICK_vSetDelay_ms(1000);

	//STATION MODE
	MUART_vSendStg((u8*)"AT+CWMODE=1\r\n");
	MYSYTICK_vSetDelay_ms(1000);
}



//connect to wifi
void HESP8266_vConnectAccessPoint(char* A_s8SSD, char* A_s8Password)
{
	//AT+CWJAP="A_s8SSD","A_s8Password"
	MUART_vSendStg((u8*)"AT+CWJAP=\"");	//"
	MUART_vSendStg((u8*)A_s8SSD);		//A_s8SSD
	MUART_vSendStg((u8*)"\",\"");	//","
	MUART_vSendStg((u8*)A_s8Password);		//A_s8Password
	MUART_vSendStg((u8*)"\"\r\n");		//"+enter+ctrl+j
	MYSYTICK_vSetDelay_ms(5000);
}



//tcp
void HESP8266_vOpenServerTCPConnection(char* IP, char* SocketNo)
{
	//AT+CIPSTART="TCP","IP", SOCKETNO
	MUART_vSendStg((u8*)"AT+CIPSTART=\"TCP\",\"");	//AT+CIPSTART="TCP","
	MUART_vSendStg((u8*)IP);		//IP
	MUART_vSendStg((u8*)"\",");	//",
	MUART_vSendStg((u8*)SocketNo);		//SocketNo
	MUART_vSendStg((u8*)"\r\n");		//enter+ctrl+j
	MYSYTICK_vSetDelay_ms(5000);
}


void HESP8266_vSendHttpRequest(char* URL)
{
	u8 numstr[6];
	//AT+CIPSEND = #
	//GET URL
	u16 URLLength = StringLength(URL) + 6;
	//http://iti-7sep.atwebpages.com/status.txt
	inttostr(URLLength,(char*)numstr);					// --------------> ???? char not u8 & char == s8
	//+ \r + \n + 4 get for and space -> size URL + 6
	MUART_vSendStg((u8*)"AT+CIPSEND=");	//AT+CIPSEND =		//7atet u8* 3ashan l function mstnya pointer
	MUART_vSendStg(numstr);		//SIZE OF THE LINE
	MUART_vSendStg((u8*)"\r\n");		//enter+ctrl+j
	MYSYTICK_vSetDelay_ms(1000);

	MUART_vSendStg((u8*)"GET");	//GET
	MUART_vSendStg((u8*)URL);		//URL
	MUART_vSendStg("\r\n");		//enter+ctrl+j

	MYSYTICK_vSetDelay_ms(2000);




}
