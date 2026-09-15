/************************************************/
/* Layer   : MCAL                               */
/* SWC     : ADC1                               */
/* File    : ADC_private.h                      */
/************************************************/
#ifndef ADC_PRIVATE_H
#define ADC_PRIVATE_H

#define ADC1_BASE_ADDRESS   0x40012400UL

typedef struct
{
    volatile u32 SR;
    volatile u32 CR1;
    volatile u32 CR2;
    volatile u32 SMPR1;
    volatile u32 SMPR2;
    volatile u32 JOFR1;
    volatile u32 JOFR2;
    volatile u32 JOFR3;
    volatile u32 JOFR4;
    volatile u32 HTR;
    volatile u32 LTR;
    volatile u32 SQR1;
    volatile u32 SQR2;
    volatile u32 SQR3;
    volatile u32 JSQR;
    volatile u32 JDR1;
    volatile u32 JDR2;
    volatile u32 JDR3;
    volatile u32 JDR4;
    volatile u32 DR;
} ADC_t;

#define ADC1  ((ADC_t*)ADC1_BASE_ADDRESS)

/* ---- SR bits ---- */
#define ADC_SR_EOC      1

/* ---- CR2 bits ---- */
#define ADC_CR2_ADON    0
#define ADC_CR2_CAL     2
#define ADC_CR2_RSTCAL  3
#define ADC_CR2_EXTTRIG 20
#define ADC_CR2_SWSTART 22

#endif
