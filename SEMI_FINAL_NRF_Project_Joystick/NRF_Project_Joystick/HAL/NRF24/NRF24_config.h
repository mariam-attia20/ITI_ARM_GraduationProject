/************************************************/
/* Layer   : HAL                                */
/* SWC     : NRF24L01(+)                        */
/* File    : NRF24_config.h                     */
/************************************************/
#ifndef NRF24_CONFIG_H
#define NRF24_CONFIG_H

/* ---- Control pins (SCK/MISO/MOSI are fixed by SPI1: PA5/PA6/PA7) ---- */
#define NRF_CE_PORT        GPIO_PORTA
#define NRF_CE_PIN         GPIO_PIN3

#define NRF_CSN_PORT       GPIO_PORTA
#define NRF_CSN_PIN        GPIO_PIN4

/* ---- RF settings : MUST be identical on TX and RX ------------------- */

/* Channel 0..125.  Each step is 1 MHz starting at 2.400 GHz.
 * Avoid 0..26 : that is WiFi channel 1 territory.
 * 108 -> 2.508 GHz, above the whole WiFi band. Good default.
 * If a neighbour uses the same band, try 76, 95 or 120.            */
#define NRF_RF_CHANNEL     108

/* Air data rate.
 * 250 kbps gives about 10 dB better sensitivity (longer range, more
 * immunity to interference) but is ONLY supported by nRF24L01+ modules.
 * Switch it on ONLY if BOTH modules are the "+" version.            */
#define NRF_DR_1MBPS       0
#define NRF_DR_250KBPS     1
#define NRF_DATA_RATE      NRF_DR_1MBPS

/* Hardware auto-acknowledge + auto-retransmit.
 * 1 = the radio itself retries a lost packet up to 15 times within
 *     ~8 ms, which kills most short interference bursts.            */
#define NRF_AUTO_ACK       1

/* Payload layout: [0] header 0xA5, [1] VRx as s8, [2] VRy as s8 */
#define NRF_PAYLOAD_SIZE   3            /* fixed payload width in bytes  */

/* 5-byte pipe address, same on both boards */
#define NRF_ADDRESS        {0xE7, 0xE7, 0xE7, 0xE7, 0xE7}

/* Simple application header byte, used to reject garbage frames */
#define NRF_MSG_HEADER     0xA5

#endif
