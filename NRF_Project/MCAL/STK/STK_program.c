/************************************************/
/* Layer   : MCAL                               */
/* SWC     : SysTick                            */
/* File    : STK_program.c                      */
/************************************************/
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../RCC/RCC_interface.h"     /* for F_CPU */
#include "STK_interface.h"
#include "STK_private.h"

void MSTK_voidInit(void)
{
    /* Disabled, no interrupt, clock source = AHB/8 */
    STK->CTRL = 0x00000000;
    STK->LOAD = 0;
    STK->VAL  = 0;
}

void MSTK_voidDelayUs(u32 Copy_u32Us)
{
    STK->LOAD = Copy_u32Us * STK_TICKS_PER_US;
    STK->VAL  = 0;
    SET_BIT(STK->CTRL, 0);                  /* enable counter            */
    while (GET_BIT(STK->CTRL, 16) == 0);    /* wait COUNTFLAG            */
    CLR_BIT(STK->CTRL, 0);                  /* disable counter           */
    STK->LOAD = 0;
    STK->VAL  = 0;
}

void MSTK_voidDelayMs(u32 Copy_u32Ms)
{
    while (Copy_u32Ms--)
    {
        MSTK_voidDelayUs(1000);
    }
}
