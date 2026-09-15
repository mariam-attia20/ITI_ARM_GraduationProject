/************************************************************/
/* Layer   : APP                                            */
/* File    : main_RX.c   -> RECEIVER BOARD                  */
/*                                                          */
/* LEDs (each: pin -> 330R -> LED anode, cathode -> GND)    */
/*   LED1 PA8  : VRx positive                               */
/*   LED2 PA9  : VRx negative                               */
/*   LED3 PA10 : VRy positive                               */
/*   LED4 PA11 : VRy negative                               */
/*                                                          */
/* nRF24  : CE  = PA3 , CSN = PA4                           */
/*          SCK = PA5 , MISO= PA6 , MOSI = PA7              */
/*                                                          */
/* An LED stays on for as long as the stick is held in that */
/* direction. The LEDs are only forced OFF after            */
/* RX_TIMEOUT_MS of complete silence, which means the       */
/* transmitter is off or out of range - not a few lost      */
/* packets.                                                 */
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

#define LED_XP_PIN        GPIO_PIN8    /* LED1 : X positive */
#define LED_XN_PIN        GPIO_PIN9    /* LED2 : X negative */
#define LED_YP_PIN        GPIO_PIN10   /* LED3 : Y positive */
#define LED_YN_PIN        GPIO_PIN11   /* LED4 : Y negative */

#define POLL_PERIOD_MS    4
#define RX_TIMEOUT_MS     1000         /* link-lost fail-safe, NOT a debounce */

int main(void)
{
    u8  Local_Au8Buffer[NRF_PAYLOAD_SIZE];
    s8  Local_s8X        = 0;
    s8  Local_s8Y        = 0;
    u16 Local_u16Silence = 0;
    u8  Local_u8Got      = 0;

    /* ---------------- Initialization ---------------- */
    MRCC_voidInitSysClock();
    MRCC_voidEnableClock(RCC_APB2, RCC_APB2_IOPA);
    MSTK_voidInit();

    MGPIO_voidSetPinMode(LED_PORT, LED_XP_PIN, GPIO_OUTPUT_PP_2MHZ);
    MGPIO_voidSetPinMode(LED_PORT, LED_XN_PIN, GPIO_OUTPUT_PP_2MHZ);
    MGPIO_voidSetPinMode(LED_PORT, LED_YP_PIN, GPIO_OUTPUT_PP_2MHZ);
    MGPIO_voidSetPinMode(LED_PORT, LED_YN_PIN, GPIO_OUTPUT_PP_2MHZ);

    MGPIO_voidSetPinValue(LED_PORT, LED_XP_PIN, GPIO_LOW);
    MGPIO_voidSetPinValue(LED_PORT, LED_XN_PIN, GPIO_LOW);
    MGPIO_voidSetPinValue(LED_PORT, LED_YP_PIN, GPIO_LOW);
    MGPIO_voidSetPinValue(LED_PORT, LED_YN_PIN, GPIO_LOW);

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
                Local_s8X   = (s8)Local_Au8Buffer[1];
                Local_s8Y   = (s8)Local_Au8Buffer[2];
                Local_u8Got = 1;
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
                /* The link really is gone. Drop everything and only now
                 * spend time checking whether the radio lost its config. */
                Local_s8X        = 0;
                Local_s8Y        = 0;
                Local_u16Silence = 0;

                if (HNRF24_u8IsAlive() == 0)
                {
                    HNRF24_voidInit();
                }
                HNRF24_voidRxMode();
            }
        }

        /* ---- drive the four LEDs ---- */
        MGPIO_voidSetPinValue(LED_PORT, LED_XP_PIN,
                              (Local_s8X > 0) ? GPIO_HIGH : GPIO_LOW);
        MGPIO_voidSetPinValue(LED_PORT, LED_XN_PIN,
                              (Local_s8X < 0) ? GPIO_HIGH : GPIO_LOW);
        MGPIO_voidSetPinValue(LED_PORT, LED_YP_PIN,
                              (Local_s8Y > 0) ? GPIO_HIGH : GPIO_LOW);
        MGPIO_voidSetPinValue(LED_PORT, LED_YN_PIN,
                              (Local_s8Y < 0) ? GPIO_HIGH : GPIO_LOW);

        MSTK_voidDelayMs(POLL_PERIOD_MS);
    }

    return 0;
}
