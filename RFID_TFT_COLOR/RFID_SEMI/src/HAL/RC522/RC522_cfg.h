/*
 * RC522_cfg.h
 *
 *  Created on: Sep 15, 2026
 *      Author: lenovo
 */

#ifndef HAL_RC522_RC522_CFG_H_
#define HAL_RC522_RC522_CFG_H_

/*
 * Wiring (STM32F1 - SPI1) :-
 * VCC  -> 3.3V
 * GND  -> GND
 * SCK  -> PA5  (SPI1_SCK)
 * MOSI -> PA7  (SPI1_MOSI)
 * MISO -> PA6  (SPI1_MISO)
 * SDA/SS/CS -> PA4  (Software CS, GPIO output)
 * RST  -> PA3  (GPIO output)
 * IRQ  -> not used
 */

#define RC522_CS_PORT		GPIO_PORTA
#define RC522_CS_PIN		GPIO_PIN4

#define RC522_RST_PORT		GPIO_PORTA
#define RC522_RST_PIN		GPIO_PIN3

#endif /* HAL_RC522_RC522_CFG_H_ */
