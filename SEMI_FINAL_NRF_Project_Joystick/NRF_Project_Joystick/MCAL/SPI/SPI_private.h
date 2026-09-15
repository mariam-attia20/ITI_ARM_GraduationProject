#ifndef SPI_PRIVATE_H
#define SPI_PRIVATE_H

#define SPI1_BASE_ADDRESS   0x40013000UL

typedef struct
{
    volatile u32 CR1;
    volatile u32 CR2;
    volatile u32 SR;
    volatile u32 DR;
    volatile u32 CRCPR;
    volatile u32 RXCRCR;
    volatile u32 TXCRCR;
    volatile u32 I2SCFGR;
    volatile u32 I2SPR;
} SPI_t;

#define SPI1   ((SPI_t*)SPI1_BASE_ADDRESS)

/* Status register bits */
#define SPI_SR_RXNE   0
#define SPI_SR_TXE    1
#define SPI_SR_BSY    7

#endif
