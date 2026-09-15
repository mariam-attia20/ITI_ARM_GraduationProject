/************************************************/
/* Layer   : HAL                                */
/* SWC     : NRF24L01(+)                        */
/* File    : NRF24_interface.h                  */
/************************************************/
#ifndef NRF24_INTERFACE_H
#define NRF24_INTERFACE_H

/* Initializes SPI1, the CE/CSN pins and the radio registers.
 * Must be called after MRCC_voidInitSysClock() and MSTK_voidInit(). */
void HNRF24_voidInit(void);

/* Puts the radio in Primary Transmitter mode */
void HNRF24_voidTxMode(void);

/* Puts the radio in Primary Receiver mode (CE stays high) */
void HNRF24_voidRxMode(void);

/* Sends NRF_PAYLOAD_SIZE bytes. Returns 1 if the packet left the FIFO. */
u8   HNRF24_u8Transmit(const u8* Copy_pu8Data);

/* Returns 1 if a packet is waiting in the RX FIFO */
u8   HNRF24_u8DataAvailable(void);

/* Reads NRF_PAYLOAD_SIZE bytes out of the RX FIFO */
void HNRF24_voidReceive(u8* Copy_pu8Buffer);

/* Reads back any register - handy to verify the SPI wiring */
u8   HNRF24_u8ReadRegister(u8 Copy_u8Register);

/* Returns 0 if the radio lost its configuration (brown-out / bad power).
 * The application should then call HNRF24_voidInit() again.          */
u8   HNRF24_u8IsAlive(void);

#endif
