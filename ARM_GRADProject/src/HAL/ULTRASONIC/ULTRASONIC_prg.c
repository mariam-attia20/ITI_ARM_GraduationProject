/*
 * ULTRASONIC_prg.c
 *
 * HC-SR04 Ultrasonic Sensor
 *
 * TRIG -> PB8
 * ECHO -> PB9
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/RCC/RCC_int.h"
#include "../../MCAL/GPIO/GPIO_int.h"

#include "ULTRASONIC_int.h"
#include "ULTRASONIC_cfg.h"


/*--------------------------------------------------*/
/* DWT Registers                                    */
/*--------------------------------------------------*/

#define DWT_BASE             0xE0001000UL
#define DWT_CTRL             (*(volatile u32 *)(DWT_BASE + 0x00))
#define DWT_CYCCNT           (*(volatile u32 *)(DWT_BASE + 0x04))


/* Core Debug registers */

#define DEMCR                (*(volatile u32 *)0xE000EDFCUL)

#define DEMCR_TRCENA         24
#define DWT_CYCCNTENA        0


/*--------------------------------------------------*/
/* Private Functions                                */
/*--------------------------------------------------*/

static void HULTRASONIC_vDWTInit(void)
{
    /*
     * Enable trace/debug block
     */
    SET_BIT(DEMCR, DEMCR_TRCENA);

    /*
     * Reset cycle counter
     */
    DWT_CYCCNT = 0;

    /*
     * Enable DWT cycle counter
     */
    SET_BIT(DWT_CTRL, DWT_CYCCNTENA);
}


/*
 * Delay using CPU cycle counter
 */
static void HULTRASONIC_vDelay_us(u32 A_u32Time_us)
{
    u32 L_u32Start;
    u32 L_u32Cycles;

    L_u32Start = DWT_CYCCNT;

    L_u32Cycles =
            A_u32Time_us * ULTRASONIC_CPU_FREQ_MHZ;

    while((DWT_CYCCNT - L_u32Start) < L_u32Cycles)
    {
        /* Wait */
    }
}


/*
 * Wait until ECHO becomes HIGH
 */
static u8 HULTRASONIC_u8WaitEchoHigh(void)
{
    u32 L_u32Start;
    u32 L_u32TimeoutCycles;

    L_u32Start = DWT_CYCCNT;

    L_u32TimeoutCycles =
            ULTRASONIC_TIMEOUT_US *
            ULTRASONIC_CPU_FREQ_MHZ;


    while(MGPIO_u8GetPinValue(
            ULTRASONIC_ECHO_PORT,
            ULTRASONIC_ECHO_PIN) == 0)
    {
        if((DWT_CYCCNT - L_u32Start) >
           L_u32TimeoutCycles)
        {
            return 0;
        }
    }

    return 1;
}


/*
 * Wait until ECHO becomes LOW
 */
static u8 HULTRASONIC_u8WaitEchoLow(void)
{
    u32 L_u32Start;
    u32 L_u32TimeoutCycles;

    L_u32Start = DWT_CYCCNT;

    L_u32TimeoutCycles =
            ULTRASONIC_TIMEOUT_US *
            ULTRASONIC_CPU_FREQ_MHZ;


    while(MGPIO_u8GetPinValue(
            ULTRASONIC_ECHO_PORT,
            ULTRASONIC_ECHO_PIN) == 1)
    {
        if((DWT_CYCCNT - L_u32Start) >
           L_u32TimeoutCycles)
        {
            return 0;
        }
    }

    return 1;
}


/*--------------------------------------------------*/
/* Public Functions                                 */
/*--------------------------------------------------*/

void HULTRASONIC_vInit(void)
{
    GPIOx_PinConfig_t L_xTrigConfig;
    GPIOx_PinConfig_t L_xEchoConfig;


    /* Enable GPIOB clock */

    MRCC_VEnableCLK(RCC_AHB1, RCC_GPIOB);


    /*----------------------------------------------*/
    /* TRIG Configuration                           */
    /*----------------------------------------------*/

    L_xTrigConfig.Port       = ULTRASONIC_TRIG_PORT;
    L_xTrigConfig.Pin        = ULTRASONIC_TRIG_PIN;
    L_xTrigConfig.Mode       = GPIO_MODE_OUTPUT;
    L_xTrigConfig.Speed      = GPIO_SPEED_LOW;
    L_xTrigConfig.OutputType = GPIO_OT_PUSHPULL;
    L_xTrigConfig.PullType   = GPIO_NO_PULL;

    MGPIO_vPinInit(&L_xTrigConfig);


    /*----------------------------------------------*/
    /* ECHO Configuration                           */
    /*----------------------------------------------*/

    L_xEchoConfig.Port       = ULTRASONIC_ECHO_PORT;
    L_xEchoConfig.Pin        = ULTRASONIC_ECHO_PIN;
    L_xEchoConfig.Mode       = GPIO_MODE_INPUT;
    L_xEchoConfig.Speed      = GPIO_SPEED_LOW;
    L_xEchoConfig.OutputType = GPIO_OT_PUSHPULL;
    L_xEchoConfig.PullType   = GPIO_NO_PULL;

    MGPIO_vPinInit(&L_xEchoConfig);


    /* TRIG initially LOW */

    MGPIO_vSetPinValue(
            ULTRASONIC_TRIG_PORT,
            ULTRASONIC_TRIG_PIN,
            GPIO_LOW);


    /* Initialize DWT */

    HULTRASONIC_vDWTInit();
}


/*--------------------------------------------------*/

u32 HULTRASONIC_u32GetDistanceCm(void)
{
    u32 L_u32Start;
    u32 L_u32End;
    u32 L_u32EchoTime;
    u32 L_u32Distance;


    /*----------------------------------------------*/
    /* Make sure ECHO is LOW                       */
    /*----------------------------------------------*/

    if(MGPIO_u8GetPinValue(
            ULTRASONIC_ECHO_PORT,
            ULTRASONIC_ECHO_PIN) == 1)
    {
        return 0;
    }


    /*----------------------------------------------*/
    /* Send trigger pulse                          */
    /*----------------------------------------------*/

    MGPIO_vSetPinValue(
            ULTRASONIC_TRIG_PORT,
            ULTRASONIC_TRIG_PIN,
            GPIO_LOW);

    HULTRASONIC_vDelay_us(2);


    MGPIO_vSetPinValue(
            ULTRASONIC_TRIG_PORT,
            ULTRASONIC_TRIG_PIN,
            GPIO_HIGH);

    HULTRASONIC_vDelay_us(10);

    MGPIO_vSetPinValue(
            ULTRASONIC_TRIG_PORT,
            ULTRASONIC_TRIG_PIN,
            GPIO_LOW);


    /*----------------------------------------------*/
    /* Wait for ECHO HIGH                          */
    /*----------------------------------------------*/

    if(HULTRASONIC_u8WaitEchoHigh() == 0)
    {
        return 0;
    }


    /*----------------------------------------------*/
    /* Start measuring ECHO HIGH time              */
    /*----------------------------------------------*/

    L_u32Start = DWT_CYCCNT;


    /* Wait for ECHO LOW */

    if(HULTRASONIC_u8WaitEchoLow() == 0)
    {
        return 0;
    }


    /*----------------------------------------------*/
    /* Calculate pulse duration                    */
    /*----------------------------------------------*/

    L_u32End = DWT_CYCCNT;

    L_u32EchoTime =
            (L_u32End - L_u32Start) /
            ULTRASONIC_CPU_FREQ_MHZ;


    /*
     * HC-SR04:
     *
     * Distance(cm) = Echo time(us) / 58
     */

    L_u32Distance =
            L_u32EchoTime / 58UL;


    return L_u32Distance;
}