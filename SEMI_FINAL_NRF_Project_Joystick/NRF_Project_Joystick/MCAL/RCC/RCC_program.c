/************************************************/
/* Layer   : MCAL                               */
/* SWC     : RCC                                */
/* File    : RCC_program.c                      */
/************************************************/
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "RCC_interface.h"
#include "RCC_private.h"

void MRCC_voidInitSysClock(void)
{
#if   RCC_CLOCK_SOURCE == RCC_HSI

    SET_BIT(RCC->CR, 0);                 /* HSI ON                        */
    while (GET_BIT(RCC->CR, 1) == 0);    /* wait HSIRDY                   */
    RCC->CFGR = 0x00000000;              /* SW = HSI, AHB/APB1/APB2 = /1  */

#elif RCC_CLOCK_SOURCE == RCC_HSE_PLL_72MHZ

    FLASH_ACR = 0x00000012;              /* prefetch ON, 2 wait states    */

    SET_BIT(RCC->CR, 16);                /* HSE ON                        */
    while (GET_BIT(RCC->CR, 17) == 0);   /* wait HSERDY                   */

    RCC->CFGR = 0;
    RCC->CFGR |= (1u << 16);             /* PLLSRC = HSE                  */
    RCC->CFGR |= (7u << 18);             /* PLLMUL = x9  -> 72 MHz        */
    RCC->CFGR |= (4u << 8);              /* APB1 = /2    -> 36 MHz        */

    SET_BIT(RCC->CR, 24);                /* PLL ON                        */
    while (GET_BIT(RCC->CR, 25) == 0);   /* wait PLLRDY                   */

    RCC->CFGR |= 2u;                     /* SW = PLL                      */
    while (((RCC->CFGR >> 2) & 3u) != 2u);

#endif
}

void MRCC_voidSetAdcPrescaler(void)
{
    /* CFGR bits 15:14 = ADCPRE.  ADC clock must never exceed 14 MHz. */
    RCC->CFGR &= ~(3u << 14);

#if RCC_CLOCK_SOURCE == RCC_HSE_PLL_72MHZ
    RCC->CFGR |= (2u << 14);      /* PCLK2 72 MHz / 6 = 12 MHz */
#else
                                  /* PCLK2  8 MHz / 2 =  4 MHz */
#endif
}

void MRCC_voidEnableClock(u8 Copy_u8Bus, u8 Copy_u8Peripheral)
{
    switch (Copy_u8Bus)
    {
        case RCC_AHB : SET_BIT(RCC->AHBENR , Copy_u8Peripheral); break;
        case RCC_APB1: SET_BIT(RCC->APB1ENR, Copy_u8Peripheral); break;
        case RCC_APB2: SET_BIT(RCC->APB2ENR, Copy_u8Peripheral); break;
        default: break;
    }
}

void MRCC_voidDisableClock(u8 Copy_u8Bus, u8 Copy_u8Peripheral)
{
    switch (Copy_u8Bus)
    {
        case RCC_AHB : CLR_BIT(RCC->AHBENR , Copy_u8Peripheral); break;
        case RCC_APB1: CLR_BIT(RCC->APB1ENR, Copy_u8Peripheral); break;
        case RCC_APB2: CLR_BIT(RCC->APB2ENR, Copy_u8Peripheral); break;
        default: break;
    }
}
