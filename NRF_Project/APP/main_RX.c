/************************************************************/
/* Layer   : APP                                            */
/* File    : main_RX.c   -> RECEIVER BOARD                  */
/*                                                          */
/* LED    : PA1 -> 330R -> LED -> GND  (HIGH = ON)          */
/* nRF24  : CE  = PA3 , CSN = PA4                           */
/*          SCK = PA5 , MISO= PA6 , MOSI = PA7              */
/*                                                          */
/* The LED holds whatever state the last valid frame said.  */
/* It is only forced OFF after RX_TIMEOUT_MS of complete    */
/* silence, which means the transmitter is off or out of    */
/* range - not a few lost packets.                          */
/************************************************************/
#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

#include "../MCAL/RCC/RCC_interface.h"
#include "../MCAL/GPIO/GPIO_interface.h"
#include "../MCAL/SPI/SPI_interface.h"
#include "../MCAL/STK/STK_interface.h"

#include "../HAL/NRF24/NRF24_interface.h"
#include "../HAL/NRF24/NRF24_config.h"

#define LED_PORT          GPIO_PORTA
#define LED_PIN           GPIO_PIN1

#define POLL_PERIOD_MS    4
#define RX_TIMEOUT_MS     1000         /* link-lost fail-safe, NOT a debounce */

int main(void)
{
    u8  Local_Au8Buffer[NRF_PAYLOAD_SIZE];
    u8  Local_u8LedState = 0;
    u16 Local_u16Silence = 0;
    u8  Local_u8Got      = 0;

    /* ---------------- Initialization ---------------- */
    MRCC_voidInitSysClock();
    MRCC_voidEnableClock(RCC_APB2, RCC_APB2_IOPA);
    MSTK_voidInit();

    MGPIO_voidSetPinMode (LED_PORT, LED_PIN, GPIO_OUTPUT_PP_2MHZ);
    MGPIO_voidSetPinValue(LED_PORT, LED_PIN, GPIO_LOW);

    HNRF24_voidInit();
    HNRF24_voidRxMode();

    /* ---------------- Super loop -------------------- */
    while (1)
    {
        /* ---- drain the FIFO, keep the newest valid frame ---- */
        Local_u8Got = 0;
        while (HNRF24_u8DataAvailable())
        {
            HNRF24_voidReceive(Local_Au8Buffer);

            if (Local_Au8Buffer[0] == NRF_MSG_HEADER)
            {
                Local_u8LedState = Local_Au8Buffer[1];
                Local_u8Got      = 1;
            }
        }

        if (Local_u8Got)
        {
            Local_u16Silence = 0;
        }
        else
        {
            Local_u16Silence += POLL_PERIOD_MS;

            if (Local_u16Silence >= RX_TIMEOUT_MS)
            {
                /* The link really is gone. Turn the LED off and only now
                 * spend time checking whether the radio lost its config. */
                Local_u8LedState = 0;
                Local_u16Silence = 0;

                if (HNRF24_u8IsAlive() == 0)
                {
                    HNRF24_voidInit();
                }
                HNRF24_voidRxMode();
            }
        }

        /* ---- drive the LED ---- */
        if (Local_u8LedState)
        {
            MGPIO_voidSetPinValue(LED_PORT, LED_PIN, GPIO_HIGH);
        }
        else
        {
            MGPIO_voidSetPinValue(LED_PORT, LED_PIN, GPIO_LOW);
        }

        MSTK_voidDelayMs(POLL_PERIOD_MS);
    }

    return 0;
}
