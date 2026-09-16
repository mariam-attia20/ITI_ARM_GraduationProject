/*
 * RC522_int.h
 *
 *  Created on: Sep 15, 2026
 *      Author: lenovo
 *
 *  HAL driver for the MFRC522 RFID reader (13.56 MHz, ISO14443A) over SPI.
 *  Sits on top of the MCAL SPI + GPIO drivers, same layering style used by
 *  the TFT HAL driver in this project.
 */

#ifndef HAL_RC522_RC522_INT_H_
#define HAL_RC522_RC522_INT_H_

/* return / status codes */
#define RC522_OK			0
#define RC522_NOTAGERR		1
#define RC522_ERR			2

#define RC522_UID_LEN		5		/* 4 UID bytes + 1 BCC checksum byte */

/* Initializes GPIO (CS/RST), SPI1 peripheral and the MFRC522 chip itself.
 * Must be called once before any other RC522 API is used. */
void HRC522_vInit(void);

/* Hardware reset of the module through the RST pin. */
void HRC522_vReset(void);

/* Re-initializes the chip's internal state (reset + timer/mode/antenna
 * registers) WITHOUT touching GPIO configuration. Call this right before
 * each detection attempt if you find the reader only detects a card once
 * and then gets stuck until a hardware reset - that means the chip's
 * internal state (timer/IRQ/error flags) isn't recovering between polls
 * on its own, and this forces it back to a clean state every cycle. */
void HRC522_vSoftReset(void);

/* Quick presence check: sends a REQA (Request type A) command.
 * Returns RC522_OK if a PICC (card/tag) answered, RC522_ERR/RC522_NOTAGERR otherwise. */
u8 HRC522_u8IsCardPresent(void);

/* Full detection: REQA followed by Anti-collision to retrieve the card UID.
 * A_pu8UID must point to a buffer of at least RC522_UID_LEN (5) bytes.
 * Returns RC522_OK on success (A_pu8UID filled with UID + checksum byte). */
u8 HRC522_u8ReadCardUID(u8* A_pu8UID);

#endif /* HAL_RC522_RC522_INT_H_ */
