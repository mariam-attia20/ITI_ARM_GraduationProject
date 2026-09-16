#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

#include "../MCAL/RCC/RCC_int.h"
#include "../MCAL/SYSTICK/SYSTICK_int.h"

#include "../HAL/blutooth/blutooth_int.h"
#include "../HAL/L298/L298_int.h"
#include "../HAL/ULTRASONIC/ULTRASONIC_int.h"


#define COMMAND_BUFFER_SIZE 8


static u8 G_u8Command = MOVE_STOP;
static u8 G_u8Speed = 0;
static u8 G_u8CommandBuffer[COMMAND_BUFFER_SIZE];
static u8 G_u8CommandIndex = 0;


static void APP_vSendUnsigned(u32 A_u32Value)
{
    u8 L_au8Digits[10];
    u8 L_u8Index = 0;

    if(A_u32Value == 0)
    {
        HBLUETOOTH_vSendChar('0');
        return;
    }

    while(A_u32Value > 0)
    {
        L_au8Digits[L_u8Index] = (u8)('0' + (A_u32Value % 10));
        A_u32Value /= 10;
        L_u8Index++;
    }

    while(L_u8Index > 0)
    {
        L_u8Index--;
        HBLUETOOTH_vSendChar(L_au8Digits[L_u8Index]);
    }
}


static void APP_vProcessCommand(void)
{
    u8 L_u8Speed = 0;
    u16 L_u16Speed = 0;
    u8 L_u8Index = 1;

    switch(G_u8CommandBuffer[0])
    {
        case 'F':
            G_u8Command = MOVE_FORWARD;
            break;

        case 'B':
            G_u8Command = MOVE_BACKWARD;
            break;

        case 'L':
            G_u8Command = MOVE_LEFT;
            break;

        case 'R':
            G_u8Command = MOVE_RIGHT;
            break;

        case 'S':
            G_u8Command = MOVE_STOP;
            G_u8Speed = 0;
            HL298_vMove(G_u8Command, G_u8Speed);
            return;

        default:
            return;
    }

    while((L_u8Index < G_u8CommandIndex) &&
          (G_u8CommandBuffer[L_u8Index] >= '0') &&
          (G_u8CommandBuffer[L_u8Index] <= '9'))
    {
        L_u16Speed = (u16)((L_u16Speed * 10) +
                           (G_u8CommandBuffer[L_u8Index] - '0'));
        L_u8Index++;

        if(L_u16Speed > 255)
        {
            L_u16Speed = 255;
        }
    }

    G_u8Speed = (u8)L_u16Speed;
}


static void APP_vReceiveCommand(void)
{
    u8 L_u8Data;

    while(HBLUETOOTH_u8ReceiveSynch(&L_u8Data))
    {
        if((L_u8Data == '\r') || (L_u8Data == '\n'))
        {
            if(G_u8CommandIndex > 0)
            {
                APP_vProcessCommand();
                G_u8CommandIndex = 0;
            }
        }
        else if(G_u8CommandIndex < COMMAND_BUFFER_SIZE)
        {
            G_u8CommandBuffer[G_u8CommandIndex] = L_u8Data;
            G_u8CommandIndex++;
        }
        else
        {
            G_u8CommandIndex = 0;
        }
    }
}


static void APP_vPublishUltrasonic(void)
{
    u32 L_u32Distance = HULTRASONIC_u32GetDistanceCm();

    HBLUETOOTH_vSendString((u8*)"D:");
    APP_vSendUnsigned(L_u32Distance);
    HBLUETOOTH_vSendString((u8*)"\r\n");
}

int main(void)
{
    MSYSTIC_Config_t L_xSysTickConfig;

    MRCC_vInit();

    HBLUETOOTH_vInit();
    HL298_vInit();
    HULTRASONIC_vInit();

    L_xSysTickConfig.InterruptEnable = INT_DISABLE;
    L_xSysTickConfig.CLK_SRC = CLK_SRC_AHB;
    MSYSTICK_vInit(&L_xSysTickConfig);

    HBLUETOOTH_vSendString((u8*)"ARM_READY\r\n");

    while(1)
    {
        APP_vReceiveCommand();
        HL298_vMove(G_u8Command, G_u8Speed);
        APP_vPublishUltrasonic();
        MSYSTICK_vSetDelay_ms(30);
    }

    return 0;
}
