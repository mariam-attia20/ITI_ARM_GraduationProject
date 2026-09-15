#ifndef GPIO_PRIVATE_H
#define GPIO_PRIVATE_H

#define GPIOA_BASE_ADDRESS   0x40010800UL
#define GPIOB_BASE_ADDRESS   0x40010C00UL
#define GPIOC_BASE_ADDRESS   0x40011000UL

typedef struct
{
    volatile u32 CRL;
    volatile u32 CRH;
    volatile u32 IDR;
    volatile u32 ODR;
    volatile u32 BSRR;
    volatile u32 BRR;
    volatile u32 LCKR;
} GPIO_t;

#define GPIOA  ((GPIO_t*)GPIOA_BASE_ADDRESS)
#define GPIOB  ((GPIO_t*)GPIOB_BASE_ADDRESS)
#define GPIOC  ((GPIO_t*)GPIOC_BASE_ADDRESS)

#endif
