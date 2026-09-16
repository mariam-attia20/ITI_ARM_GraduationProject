#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

#include "../MCAL/RCC/RCC_int.h"

#include "../HAL/blutooth/blutooth_int.h"
#include "../HAL/L298/L298_int.h"

#include "../APP/REMOTE_XY/REMOTE_XY_int.h"

int main(void)
{
    u8 L_u8Data;

    MRCC_vInit();

    HBLUETOOTH_vInit();

    HBLUETOOTH_vSendString((u8*)"TEST\r\n");

    while(1)
    {
        if(HBLUETOOTH_u8ReceiveSynch(&L_u8Data))
        {
            HBLUETOOTH_vSendChar(L_u8Data);
        }
    }
}
