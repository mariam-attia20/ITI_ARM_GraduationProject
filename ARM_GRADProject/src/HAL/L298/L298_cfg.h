/*
 * L298_cfg.h
 *
 *  Created on: 7 Sep 2026
 *      Author: LOQ
 */

#ifndef DIVERS_HAL_L298_L298_CFG_H_
#define DIVERS_HAL_L298_L298_CFG_H_


/* ================= MOTOR 1 ================= */

#define M1_IN1_PORT        GPIO_PORTA
#define M1_IN1_PIN         GPIO_PIN0

#define M1_IN2_PORT        GPIO_PORTA
#define M1_IN2_PIN         GPIO_PIN1

#define M1_EN_PORT         GPIO_PORTA
#define M1_EN_PIN          GPIO_PIN6


/* ================= MOTOR 2 ================= */

#define M2_IN1_PORT        GPIO_PORTA
#define M2_IN1_PIN         GPIO_PIN2

#define M2_IN2_PORT        GPIO_PORTA
#define M2_IN2_PIN         GPIO_PIN3

#define M2_EN_PORT         GPIO_PORTA
#define M2_EN_PIN          GPIO_PIN7


/* ================= MOTOR 3 ================= */

#define M3_IN1_PORT        GPIO_PORTB
#define M3_IN1_PIN         GPIO_PIN0

#define M3_IN2_PORT        GPIO_PORTB
#define M3_IN2_PIN         GPIO_PIN1

#define M3_EN_PORT         GPIO_PORTB
#define M3_EN_PIN          GPIO_PIN6


/* ================= MOTOR 4 ================= */

#define M4_IN1_PORT        GPIO_PORTB
#define M4_IN1_PIN         GPIO_PIN2

#define M4_IN2_PORT        GPIO_PORTB
#define M4_IN2_PIN         GPIO_PIN3

#define M4_EN_PORT         GPIO_PORTB
#define M4_EN_PIN          GPIO_PIN7


#endif /* DIVERS_HAL_L298_L298_CFG_H_ */


