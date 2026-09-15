/************************************************/
/* Layer   : MCAL                               */
/* SWC     : RCC                                */
/* File    : RCC_interface.h                    */
/************************************************/
#ifndef RCC_INTERFACE_H
#define RCC_INTERFACE_H

/* ---- Clock source selection ------------------------------------ */
#define RCC_HSI              0   /* internal 8 MHz  (no crystal needed) */
#define RCC_HSE_PLL_72MHZ    1   /* external 8 MHz crystal * 9 = 72 MHz */

#define RCC_CLOCK_SOURCE     RCC_HSI

#if   RCC_CLOCK_SOURCE == RCC_HSI
#define F_CPU                8000000UL
#elif RCC_CLOCK_SOURCE == RCC_HSE_PLL_72MHZ
#define F_CPU                72000000UL
#endif

/* ---- Buses ------------------------------------------------------ */
#define RCC_AHB    0
#define RCC_APB1   1
#define RCC_APB2   2

/* ---- APB2 peripheral bit positions ------------------------------ */
#define RCC_APB2_AFIO    0
#define RCC_APB2_IOPA    2
#define RCC_APB2_IOPB    3
#define RCC_APB2_IOPC    4
#define RCC_APB2_ADC1    9
#define RCC_APB2_SPI1   12
#define RCC_APB2_USART1 14

/* ---- APB1 peripheral bit positions ------------------------------ */
#define RCC_APB1_TIM2    0
#define RCC_APB1_SPI2   14
#define RCC_APB1_USART2 17
#define RCC_APB1_I2C1   21

void MRCC_voidInitSysClock(void);

/* Sets the ADC prescaler so that the ADC clock stays within the 14 MHz limit
 * for whichever RCC_CLOCK_SOURCE is selected above. Called by MADC1_voidInit. */
void MRCC_voidSetAdcPrescaler(void);
void MRCC_voidEnableClock (u8 Copy_u8Bus, u8 Copy_u8Peripheral);
void MRCC_voidDisableClock(u8 Copy_u8Bus, u8 Copy_u8Peripheral);

#endif
