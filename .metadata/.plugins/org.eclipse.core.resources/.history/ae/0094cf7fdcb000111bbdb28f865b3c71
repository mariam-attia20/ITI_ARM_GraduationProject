/*
 * BLUETOOTH_prg.c
 *
 * Created on: Sep 15, 2026
 * Author: LQ
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/USART/USART_int.h"

#include "BLUETOOTH_int.h"


void HBLUETOOTH_vInit(void)
{
    /* Initialize USART used with Bluetooth module */
    MUSART_vInit();
}


void HBLUETOOTH_vSendChar(u8 A_u8Data)
{
    MUSART_vTransmit(A_u8Data);
}


void HBLUETOOTH_vSendString(const u8 A_u8String[])
{
    u8 L_u8Index = 0;

    while(A_u8String[L_u8Index] != '\0')
    {
        HBLUETOOTH_vSendChar(A_u8String[L_u8Index]);

        L_u8Index++;
    }
}


u8 HBLUETOOTH_u8ReceiveChar(void)
{
    return MUSART_u8Receive();
}


u8 HBLUETOOTH_u8DataAvailable(void)
{
    return MUSART_u8ReceiveStatus();
}