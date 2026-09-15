/************************************************/
/* Layer   : MCAL                               */
/* SWC     : GPIO                               */
/* File    : GPIO_interface.h                   */
/************************************************/
#ifndef GPIO_INTERFACE_H
#define GPIO_INTERFACE_H

#define GPIO_PORTA   0
#define GPIO_PORTB   1
#define GPIO_PORTC   2

#define GPIO_PIN0    0
#define GPIO_PIN1    1
#define GPIO_PIN2    2
#define GPIO_PIN3    3
#define GPIO_PIN4    4
#define GPIO_PIN5    5
#define GPIO_PIN6    6
#define GPIO_PIN7    7
#define GPIO_PIN8    8
#define GPIO_PIN9    9
#define GPIO_PIN10  10
#define GPIO_PIN11  11
#define GPIO_PIN12  12
#define GPIO_PIN13  13
#define GPIO_PIN14  14
#define GPIO_PIN15  15

/* CNF[1:0] : MODE[1:0]  -> 4-bit configuration nibble */
#define GPIO_INPUT_ANALOG            0b0000
#define GPIO_INPUT_FLOATING          0b0100
#define GPIO_INPUT_PULL_UP_DOWN      0b1000

#define GPIO_OUTPUT_PP_2MHZ          0b0010
#define GPIO_OUTPUT_PP_10MHZ         0b0001
#define GPIO_OUTPUT_PP_50MHZ         0b0011
#define GPIO_OUTPUT_OD_2MHZ          0b0110

#define GPIO_AF_PP_2MHZ              0b1010
#define GPIO_AF_PP_10MHZ             0b1001
#define GPIO_AF_PP_50MHZ             0b1011
#define GPIO_AF_OD_2MHZ              0b1110

#define GPIO_LOW     0
#define GPIO_HIGH    1

void MGPIO_voidSetPinMode (u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8Mode);
void MGPIO_voidSetPinValue(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8Value);
u8   MGPIO_u8GetPinValue  (u8 Copy_u8Port, u8 Copy_u8Pin);

#endif
