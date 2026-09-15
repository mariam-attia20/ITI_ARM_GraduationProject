/************************************************/
/* Layer   : HAL                                */
/* SWC     : NRF24L01(+)                        */
/* File    : NRF24_private.h                    */
/************************************************/
#ifndef NRF24_PRIVATE_H
#define NRF24_PRIVATE_H

/* ---- Register map ---- */
#define NRF_REG_CONFIG        0x00
#define NRF_REG_EN_AA         0x01
#define NRF_REG_EN_RXADDR     0x02
#define NRF_REG_SETUP_AW      0x03
#define NRF_REG_SETUP_RETR    0x04
#define NRF_REG_RF_CH         0x05
#define NRF_REG_RF_SETUP      0x06
#define NRF_REG_STATUS        0x07
#define NRF_REG_OBSERVE_TX    0x08
#define NRF_REG_RPD           0x09
#define NRF_REG_RX_ADDR_P0    0x0A
#define NRF_REG_TX_ADDR       0x10
#define NRF_REG_RX_PW_P0      0x11
#define NRF_REG_FIFO_STATUS   0x17
#define NRF_REG_DYNPD         0x1C
#define NRF_REG_FEATURE       0x1D

/* ---- SPI commands ---- */
#define NRF_CMD_R_REGISTER    0x00
#define NRF_CMD_W_REGISTER    0x20
#define NRF_CMD_R_RX_PAYLOAD  0x61
#define NRF_CMD_W_TX_PAYLOAD  0xA0
#define NRF_CMD_FLUSH_TX      0xE1
#define NRF_CMD_FLUSH_RX      0xE2
#define NRF_CMD_NOP           0xFF

/* ---- CONFIG register bits ---- */
#define NRF_CONFIG_PRIM_RX    0
#define NRF_CONFIG_PWR_UP     1
#define NRF_CONFIG_CRCO       2
#define NRF_CONFIG_EN_CRC     3

/* ---- STATUS register bits ---- */
#define NRF_STATUS_TX_FULL    0
#define NRF_STATUS_MAX_RT     4
#define NRF_STATUS_TX_DS      5
#define NRF_STATUS_RX_DR      6

/* ---- FIFO_STATUS bits ---- */
#define NRF_FIFO_RX_EMPTY     0

#endif
