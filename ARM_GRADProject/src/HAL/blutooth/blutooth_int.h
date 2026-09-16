#ifndef BLUETOOTH_INT_H_
#define BLUETOOTH_INT_H_

#include "../../LIB/STD_TYPES.h"


void HBLUETOOTH_vInit(void);

void HBLUETOOTH_vSendChar(u8 A_u8Data);

void HBLUETOOTH_vSendString(const u8 *A_u8String);

u8 HBLUETOOTH_u8ReceiveChar(void);

u8 HBLUETOOTH_u8ReceiveSynch(u8 *A_pu8Data);


#endif
