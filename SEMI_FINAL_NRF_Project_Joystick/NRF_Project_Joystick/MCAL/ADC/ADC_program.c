/************************************************/
/* Layer   : MCAL                               */
/* SWC     : ADC1                               */
/* File    : ADC_program.c                      */
/************************************************/
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../RCC/RCC_interface.h"
#include "../STK/STK_interface.h"

#include "ADC_interface.h"
#include "ADC_private.h"

void MADC1_voidInit(void)
{
    /* 1- Clock for the peripheral, and a legal ADC clock (<= 14 MHz) */
    MRCC_voidEnableClock(RCC_APB2, RCC_APB2_ADC1);
    MRCC_voidSetAdcPrescaler();

    /* 2- Independent mode, no scan, no interrupt */
    ADC1->CR1 = 0;
    ADC1->CR2 = 0;

    /* 3- Trigger the conversion from software (SWSTART).
     *    EXTSEL = 111 and EXTTRIG = 1 is the documented way to do this;
     *    writing ADON twice also starts a conversion but is race prone. */
    ADC1->CR2 |= (7u << 17);
    SET_BIT(ADC1->CR2, ADC_CR2_EXTTRIG);

    /* 4- One conversion per sequence (L = 0) */
    ADC1->SQR1 = 0;

    /* 5- 239.5-cycle sample time on every channel.
     *    A joystick potentiometer is a high impedance source; the default
     *    1.5-cycle sample time gives readings that wander by 100+ counts. */
    ADC1->SMPR2 = 0x3FFFFFFFu;   /* channels 0..9  */
    ADC1->SMPR1 = 0x00FFFFFFu;   /* channels 10..17 */

    /* 6- Wake the analog block up, then let it stabilise */
    SET_BIT(ADC1->CR2, ADC_CR2_ADON);
    MSTK_voidDelayUs(20);

    /* 7- Self calibration - removes the internal capacitor offset */
    SET_BIT(ADC1->CR2, ADC_CR2_RSTCAL);
    while (GET_BIT(ADC1->CR2, ADC_CR2_RSTCAL) != 0);

    SET_BIT(ADC1->CR2, ADC_CR2_CAL);
    while (GET_BIT(ADC1->CR2, ADC_CR2_CAL) != 0);
}

u16 MADC1_u16ReadChannel(u8 Copy_u8Channel)
{
    /* Single conversion sequence = just this channel */
    ADC1->SQR3 = (u32)(Copy_u8Channel & 0x1Fu);

    ADC1->SR = 0;
    SET_BIT(ADC1->CR2, ADC_CR2_SWSTART);

    while (GET_BIT(ADC1->SR, ADC_SR_EOC) == 0);

    return (u16)(ADC1->DR & 0x0FFFu);   /* reading DR clears EOC */
}
