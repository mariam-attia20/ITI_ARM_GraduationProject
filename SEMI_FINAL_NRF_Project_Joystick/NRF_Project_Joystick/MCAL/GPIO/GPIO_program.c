/************************************************/
/* Layer   : MCAL                               */
/* SWC     : GPIO                               */
/* File    : GPIO_program.c                     */
/************************************************/
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "GPIO_interface.h"
#include "GPIO_private.h"

static GPIO_t* GPIO_ptrGetPort(u8 Copy_u8Port)
{
    switch (Copy_u8Port)
    {
        case GPIO_PORTA: return GPIOA;
        case GPIO_PORTB: return GPIOB;
        case GPIO_PORTC: return GPIOC;
        default        : return GPIOA;
    }
}

void MGPIO_voidSetPinMode(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8Mode)
{
    GPIO_t* Local_ptrPort = GPIO_ptrGetPort(Copy_u8Port);

    if (Copy_u8Pin < 8)
    {
        Local_ptrPort->CRL &= ~(0x0Fu << (Copy_u8Pin * 4));
        Local_ptrPort->CRL |=  ((u32)(Copy_u8Mode & 0x0Fu) << (Copy_u8Pin * 4));
    }
    else
    {
        Copy_u8Pin -= 8;
        Local_ptrPort->CRH &= ~(0x0Fu << (Copy_u8Pin * 4));
        Local_ptrPort->CRH |=  ((u32)(Copy_u8Mode & 0x0Fu) << (Copy_u8Pin * 4));
    }
}

void MGPIO_voidSetPinValue(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8Value)
{
    GPIO_t* Local_ptrPort = GPIO_ptrGetPort(Copy_u8Port);

    if (Copy_u8Value == GPIO_HIGH)
    {
        Local_ptrPort->BSRR = (1u << Copy_u8Pin);
    }
    else
    {
        Local_ptrPort->BRR  = (1u << Copy_u8Pin);
    }
}

u8 MGPIO_u8GetPinValue(u8 Copy_u8Port, u8 Copy_u8Pin)
{
    GPIO_t* Local_ptrPort = GPIO_ptrGetPort(Copy_u8Port);
    return (u8)GET_BIT(Local_ptrPort->IDR, Copy_u8Pin);
}
