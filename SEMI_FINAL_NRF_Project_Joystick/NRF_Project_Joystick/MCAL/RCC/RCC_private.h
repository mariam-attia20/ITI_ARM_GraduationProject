#ifndef RCC_PRIVATE_H
#define RCC_PRIVATE_H

#define RCC_BASE_ADDRESS    0x40021000UL
#define FLASH_ACR           (*((volatile u32*)0x40022000UL))

typedef struct
{
    volatile u32 CR;
    volatile u32 CFGR;
    volatile u32 CIR;
    volatile u32 APB2RSTR;
    volatile u32 APB1RSTR;
    volatile u32 AHBENR;
    volatile u32 APB2ENR;
    volatile u32 APB1ENR;
    volatile u32 BDCR;
    volatile u32 CSR;
} RCC_t;

#define RCC   ((RCC_t*)RCC_BASE_ADDRESS)

#endif
