/************************************************************/
/* Layer   : APP                                            */
/* File    : main_TX.c   -> TRANSMITTER BOARD               */
/*                                                          */
/* Button : PA0  (button between PA0 and GND, internal      */
/*                pull-up enabled -> pressed reads 0)       */
/* nRF24  : CE  = PA3 , CSN = PA4                           */
/*          SCK = PA5 , MISO= PA6 , MOSI = PA7              */
/*                                                          */
/* Behaviour:                                               */
/*  - a PRESS is accepted immediately (no latency)          */
/*  - a RELEASE is only accepted after the button has been   */
/*    steadily open for RELEASE_CONFIRM_MS, so contact       */
/*    chatter can never switch the LED off mid-press        */
/*  - every state CHANGE is sent as a burst of frames, so a  */
/*    release is never carried by one single packet         */
/*  - the steady state keeps streaming as a keep-alive       */
/************************************************************/
#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

#include "../MCAL/RCC/RCC_interface.h"
#include "../MCAL/GPIO/GPIO_interface.h"
#include "../MCAL/SPI/SPI_interface.h"
#include "../MCAL/STK/STK_interface.h"

#include "../HAL/NRF24/NRF24_interface.h"
#include "../HAL/NRF24/NRF24_config.h"

#define BUTTON_PORT          GPIO_PORTA
#define BUTTON_PIN           GPIO_PIN0
#define BUTTON_PRESSED       0         /* active low */

#define LOOP_PERIOD_MS       10        /* keep-alive frame every 10 ms      */
#define BURST_PERIOD_MS      2         /* frame spacing inside a burst      */
#define BURST_FRAMES         6         /* frames sent on every state change */

#define RELEASE_CONFIRM_MS   50        /* button must stay open this long   */
#define RELEASE_CONFIRM_CNT  (RELEASE_CONFIRM_MS / LOOP_PERIOD_MS)

#define FAIL_LIMIT           80        /* failed sends before health check  */

int main(void)
{
    u8 Local_Au8Payload[NRF_PAYLOAD_SIZE];
    u8 Local_u8Raw;
    u8 Local_u8State        = 0;       /* debounced button state */
    u8 Local_u8LastState    = 0;
    u8 Local_u8ReleaseCount = 0;
    u8 Local_u8Burst        = 0;
    u8 Local_u8FailCount    = 0;

    /* ---------------- Initialization ---------------- */
    MRCC_voidInitSysClock();
    MRCC_voidEnableClock(RCC_APB2, RCC_APB2_IOPA);
    MSTK_voidInit();

    MGPIO_voidSetPinMode (BUTTON_PORT, BUTTON_PIN, GPIO_INPUT_PULL_UP_DOWN);
    MGPIO_voidSetPinValue(BUTTON_PORT, BUTTON_PIN, GPIO_HIGH);   /* pull-up */

    HNRF24_voidInit();
    HNRF24_voidTxMode();

    /* ---------------- Super loop -------------------- */
    while (1)
    {
        /* ---- asymmetric debounce ---- */
        Local_u8Raw = (MGPIO_u8GetPinValue(BUTTON_PORT, BUTTON_PIN) == BUTTON_PRESSED);

        if (Local_u8Raw)
        {
            Local_u8State        = 1;          /* press wins instantly */
            Local_u8ReleaseCount = 0;
        }
        else if (Local_u8State)
        {
            Local_u8ReleaseCount++;
            if (Local_u8ReleaseCount >= RELEASE_CONFIRM_CNT)
            {
                Local_u8State = 0;             /* confirmed release */
            }
        }

        /* ---- burst on every change ---- */
        if (Local_u8State != Local_u8LastState)
        {
            Local_u8LastState = Local_u8State;
            Local_u8Burst     = BURST_FRAMES;
        }

        /* ---- send ---- */
        Local_Au8Payload[0] = NRF_MSG_HEADER;
        Local_Au8Payload[1] = Local_u8State;

        if (HNRF24_u8Transmit(Local_Au8Payload))
        {
            Local_u8FailCount = 0;
        }
        else if (Local_u8FailCount < FAIL_LIMIT)
        {
            Local_u8FailCount++;
        }
        else
        {
            /* Only now, with the link clearly dead, is it worth paying the
             * 100 ms cost of a full re-init. Never touch a working link. */
            Local_u8FailCount = 0;
            if (HNRF24_u8IsAlive() == 0)
            {
                HNRF24_voidInit();
                HNRF24_voidTxMode();
            }
        }

        if (Local_u8Burst)
        {
            Local_u8Burst--;
            MSTK_voidDelayMs(BURST_PERIOD_MS);
        }
        else
        {
            MSTK_voidDelayMs(LOOP_PERIOD_MS);
        }
    }

    return 0;
}
