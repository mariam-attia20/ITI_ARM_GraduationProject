/************************************************************/
/* Layer   : APP                                            */
/* File    : main_TX.c   -> TRANSMITTER BOARD               */
/*                                                          */
/* Joystick : PB0 (analog) -> VRx   [ADC1 channel 8]        */
/*            PB1 (analog) -> VRy   [ADC1 channel 9]        */
/*            joystick VCC -> 3.3V , GND -> GND             */
/*                                                          */
/* nRF24   : CE  = PA3 , CSN = PA4                          */
/*           SCK = PA5 , MISO= PA6 , MOSI = PA7             */
/*                                                          */
/* NOTE ON PINS: PB6 / PB7 have NO ADC channel on the       */
/* STM32F103. ADC1 only reaches PA0..PA7 (IN0..IN7) and     */
/* PB0/PB1 (IN8/IN9). PB0 and PB1 are the only analog       */
/* capable pins on port B, so they are used here.           */
/*                                                          */
/* Output value per axis:                                   */
/*   centered            ->    0                            */
/*   pushed +ve          ->  +1 .. +125                     */
/*   pushed -ve          ->  -1 .. -125                     */
/*                                                          */
/* Same concept as the button version:                      */
/*  - leaving the center is accepted immediately            */
/*  - returning to center is only accepted after the axis    */
/*    has been steadily centered for CENTER_CONFIRM_MS, so   */
/*    noise around the deadzone edge cannot flicker the LED  */
/*  - every direction CHANGE is sent as a burst of frames    */
/*  - the steady state keeps streaming as a keep-alive       */
/************************************************************/
#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

#include "../MCAL/RCC/RCC_interface.h"
#include "../MCAL/GPIO/GPIO_interface.h"
#include "../MCAL/SPI/SPI_interface.h"
#include "../MCAL/STK/STK_interface.h"
#include "../MCAL/ADC/ADC_interface.h"

#include "../HAL/NRF24/NRF24_interface.h"
#include "../HAL/NRF24/NRF24_config.h"

/* ---------------- Joystick wiring ---------------- */
#define JOY_X_PORT           GPIO_PORTB
#define JOY_X_PIN            GPIO_PIN0
#define JOY_X_CHANNEL        ADC1_CH8

#define JOY_Y_PORT           GPIO_PORTB
#define JOY_Y_PIN            GPIO_PIN1
#define JOY_Y_CHANNEL        ADC1_CH9

/* Set to 1 if an axis moves the wrong way round for your module */
#define JOY_X_INVERT         0
#define JOY_Y_INVERT         0

/* ---------------- Scaling ------------------------ */
#define AXIS_OUT_MAX         125      /* full scale of the transmitted value */

/* Counts around the measured center that still report 0.
 * Raise it if the LEDs light up with the stick at rest,
 * lower it if the stick has to travel too far before reacting. */
#define AXIS_DEADZONE        150

/* Samples averaged per reading, and samples averaged at startup
 * to learn where this particular joystick's center actually is. */
#define AXIS_OVERSAMPLE      4
#define CENTER_SAMPLES       64

/* ---------------- Timing ------------------------- */
#define LOOP_PERIOD_MS       10       /* keep-alive frame every 10 ms      */
#define BURST_PERIOD_MS      2        /* frame spacing inside a burst      */
#define BURST_FRAMES         6        /* frames sent on every state change */

#define CENTER_CONFIRM_MS    50       /* axis must stay centered this long */
#define CENTER_CONFIRM_CNT   (CENTER_CONFIRM_MS / LOOP_PERIOD_MS)

#define FAIL_LIMIT           80       /* failed sends before health check  */

#define AXIS_X               0
#define AXIS_Y               1

/* Per-axis calibration, filled in at startup */
static u16 Global_Au16Center [2];
static u16 Global_Au16SpanPos[2];
static u16 Global_Au16SpanNeg[2];

static const u8 Global_Au8Channel[2] = { JOY_X_CHANNEL, JOY_Y_CHANNEL };
static const u8 Global_Au8Invert [2] = { JOY_X_INVERT , JOY_Y_INVERT  };

/* ------------------------------------------------------------------ */
/* Averaged raw reading of one axis                                    */
/* ------------------------------------------------------------------ */
static u16 APP_u16ReadAxisRaw(u8 Copy_u8Axis)
{
    u32 Local_u32Sum = 0;
    u8  Local_u8Idx;

    for (Local_u8Idx = 0; Local_u8Idx < AXIS_OVERSAMPLE; Local_u8Idx++)
    {
        Local_u32Sum += MADC1_u16ReadChannel(Global_Au8Channel[Copy_u8Axis]);
    }

    return (u16)(Local_u32Sum / AXIS_OVERSAMPLE);
}

/* ------------------------------------------------------------------ */
/* Learns the resting position of the stick.                           */
/* DO NOT TOUCH THE JOYSTICK while the board powers up.                */
/* ------------------------------------------------------------------ */
static void APP_voidCalibrateCenter(void)
{
    u32 Local_u32Sum;
    u8  Local_u8Axis;
    u16 Local_u16Idx;

    for (Local_u8Axis = 0; Local_u8Axis < 2; Local_u8Axis++)
    {
        Local_u32Sum = 0;

        for (Local_u16Idx = 0; Local_u16Idx < CENTER_SAMPLES; Local_u16Idx++)
        {
            Local_u32Sum += MADC1_u16ReadChannel(Global_Au8Channel[Local_u8Axis]);
        }

        Global_Au16Center[Local_u8Axis] = (u16)(Local_u32Sum / CENTER_SAMPLES);

        /* The two halves of the travel are rarely equal, so each one gets
         * its own span. That is what makes both directions reach 125.     */
        Global_Au16SpanPos[Local_u8Axis] = 4095u - Global_Au16Center[Local_u8Axis];
        Global_Au16SpanNeg[Local_u8Axis] =         Global_Au16Center[Local_u8Axis];
    }
}

/* ------------------------------------------------------------------ */
/* Raw 0..4095 -> -125..0..+125                                        */
/* ------------------------------------------------------------------ */
static s8 APP_s8ScaleAxis(u8 Copy_u8Axis, u16 Copy_u16Raw)
{
    s32 Local_s32Delta;
    s32 Local_s32Span;
    s32 Local_s32Mag;

    Local_s32Delta = (s32)Copy_u16Raw - (s32)Global_Au16Center[Copy_u8Axis];

    if (Global_Au8Invert[Copy_u8Axis])
    {
        Local_s32Delta = -Local_s32Delta;
    }

    if (Local_s32Delta > AXIS_DEADZONE)
    {
        Local_s32Mag  = Local_s32Delta - AXIS_DEADZONE;
        Local_s32Span = (s32)(Global_Au8Invert[Copy_u8Axis]
                              ? Global_Au16SpanNeg[Copy_u8Axis]
                              : Global_Au16SpanPos[Copy_u8Axis]);
    }
    else if (Local_s32Delta < -AXIS_DEADZONE)
    {
        Local_s32Mag  = -Local_s32Delta - AXIS_DEADZONE;
        Local_s32Span = (s32)(Global_Au8Invert[Copy_u8Axis]
                              ? Global_Au16SpanPos[Copy_u8Axis]
                              : Global_Au16SpanNeg[Copy_u8Axis]);
    }
    else
    {
        return 0;                       /* centered */
    }

    Local_s32Span -= AXIS_DEADZONE;
    if (Local_s32Span < 1)
    {
        Local_s32Span = 1;              /* guard against a bad calibration */
    }

    if (Local_s32Mag > Local_s32Span)
    {
        Local_s32Mag = Local_s32Span;
    }

    Local_s32Mag = (Local_s32Mag * AXIS_OUT_MAX) / Local_s32Span;

    if (Local_s32Mag > AXIS_OUT_MAX)
    {
        Local_s32Mag = AXIS_OUT_MAX;
    }
    if (Local_s32Mag < 1)
    {
        Local_s32Mag = 1;               /* outside the deadzone is never 0 */
    }

    return (Local_s32Delta > 0) ? (s8)Local_s32Mag : (s8)(-Local_s32Mag);
}

/* ------------------------------------------------------------------ */
/* bit0 X+, bit1 X-, bit2 Y+, bit3 Y-  : what the receiver will light  */
/* ------------------------------------------------------------------ */
static u8 APP_u8DirectionMask(s8 Copy_s8X, s8 Copy_s8Y)
{
    u8 Local_u8Mask = 0;

    if (Copy_s8X > 0) { Local_u8Mask |= 0x01u; }
    if (Copy_s8X < 0) { Local_u8Mask |= 0x02u; }
    if (Copy_s8Y > 0) { Local_u8Mask |= 0x04u; }
    if (Copy_s8Y < 0) { Local_u8Mask |= 0x08u; }

    return Local_u8Mask;
}

int main(void)
{
    u8  Local_Au8Payload[NRF_PAYLOAD_SIZE];
    s8  Local_As8Out[2]       = { 0, 0 };   /* value actually transmitted */
    u8  Local_Au8CenterCnt[2] = { 0, 0 };
    s8  Local_s8Fresh;
    u8  Local_u8Axis;
    u8  Local_u8Mask;
    u8  Local_u8LastMask  = 0;
    u8  Local_u8Burst     = 0;
    u8  Local_u8FailCount = 0;

    /* ---------------- Initialization ---------------- */
    MRCC_voidInitSysClock();
    MRCC_voidEnableClock(RCC_APB2, RCC_APB2_IOPA);
    MRCC_voidEnableClock(RCC_APB2, RCC_APB2_IOPB);
    MSTK_voidInit();

    MGPIO_voidSetPinMode(JOY_X_PORT, JOY_X_PIN, GPIO_INPUT_ANALOG);
    MGPIO_voidSetPinMode(JOY_Y_PORT, JOY_Y_PIN, GPIO_INPUT_ANALOG);

    MADC1_voidInit();
    APP_voidCalibrateCenter();          /* stick must be at rest here */

    HNRF24_voidInit();
    HNRF24_voidTxMode();

    /* ---------------- Super loop -------------------- */
    while (1)
    {
        /* ---- read both axes, asymmetric center debounce ---- */
        for (Local_u8Axis = 0; Local_u8Axis < 2; Local_u8Axis++)
        {
            Local_s8Fresh = APP_s8ScaleAxis(Local_u8Axis,
                                            APP_u16ReadAxisRaw(Local_u8Axis));

            if (Local_s8Fresh != 0)
            {
                Local_As8Out[Local_u8Axis]       = Local_s8Fresh;  /* instant */
                Local_Au8CenterCnt[Local_u8Axis] = 0;
            }
            else if (Local_As8Out[Local_u8Axis] != 0)
            {
                Local_Au8CenterCnt[Local_u8Axis]++;
                if (Local_Au8CenterCnt[Local_u8Axis] >= CENTER_CONFIRM_CNT)
                {
                    Local_As8Out[Local_u8Axis] = 0;   /* confirmed centered */
                }
            }
        }

        /* ---- burst on every direction change ---- */
        Local_u8Mask = APP_u8DirectionMask(Local_As8Out[AXIS_X],
                                           Local_As8Out[AXIS_Y]);

        if (Local_u8Mask != Local_u8LastMask)
        {
            Local_u8LastMask = Local_u8Mask;
            Local_u8Burst    = BURST_FRAMES;
        }

        /* ---- send ---- */
        Local_Au8Payload[0] = NRF_MSG_HEADER;
        Local_Au8Payload[1] = (u8)Local_As8Out[AXIS_X];   /* two's complement */
        Local_Au8Payload[2] = (u8)Local_As8Out[AXIS_Y];

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
