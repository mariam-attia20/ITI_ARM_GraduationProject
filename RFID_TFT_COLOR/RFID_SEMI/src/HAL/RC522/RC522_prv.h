/*
 * RC522_prv.h
 *
 *  Created on: Sep 15, 2026
 *      Author: lenovo
 */

#ifndef HAL_RC522_RC522_PRV_H_
#define HAL_RC522_RC522_PRV_H_

/* ---------------- MFRC522 register map (page 0/1/2/3 addresses, 6-bit) --------------- */
#define REG_COMMAND			0x01
#define REG_COM_IEN			0x02
#define REG_DIV_IEN			0x03
#define REG_COM_IRQ			0x04
#define REG_DIV_IRQ			0x05
#define REG_ERROR			0x06
#define REG_STATUS1			0x07
#define REG_STATUS2			0x08
#define REG_FIFO_DATA		0x09
#define REG_FIFO_LEVEL		0x0A
#define REG_WATER_LEVEL		0x0B
#define REG_CONTROL			0x0C
#define REG_BIT_FRAMING		0x0D
#define REG_COLL			0x0E

#define REG_MODE			0x11
#define REG_TX_MODE			0x12
#define REG_RX_MODE			0x13
#define REG_TX_CONTROL		0x14
#define REG_TX_AUTO			0x15
#define REG_TX_SEL			0x16
#define REG_RX_SEL			0x17
#define REG_RX_THRESHOLD	0x18
#define REG_DEMOD			0x19

#define REG_CRC_RESULT_M	0x21
#define REG_CRC_RESULT_L	0x22
#define REG_MOD_WIDTH		0x24
#define REG_RF_CFG			0x26
#define REG_GS_N			0x27
#define REG_CW_GS_P			0x28
#define REG_MOD_GS_P		0x29
#define REG_T_MODE			0x2A
#define REG_T_PRESCALER		0x2B
#define REG_T_RELOAD_H		0x2C
#define REG_T_RELOAD_L		0x2D

#define REG_VERSION			0x37

/* ---------------- PCD (reader) commands ---------------- */
#define PCD_IDLE			0x00
#define PCD_AUTHENT			0x0E
#define PCD_RECEIVE			0x08
#define PCD_TRANSMIT		0x04
#define PCD_TRANSCEIVE		0x0C
#define PCD_RESETPHASE		0x0F
#define PCD_CALCCRC			0x03

/* ---------------- PICC (card) commands ---------------- */
#define PICC_REQIDL			0x26	/* REQA - wakes idle cards */
#define PICC_REQALL			0x52	/* WUPA - wakes all cards (idle + halt) */
#define PICC_ANTICOLL		0x93	/* Anti-collision, cascade level 1 */

#endif /* HAL_RC522_RC522_PRV_H_ */
