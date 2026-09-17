/*
 * ULTRASONIC_cfg.h
 */

#ifndef HAL_ULTRASONIC_ULTRASONIC_CFG_H_
#define HAL_ULTRASONIC_ULTRASONIC_CFG_H_

#include "../../CONFIG/PinConfig.h"

/*
 * STM32F401 system clock
 *
 * Change this value if your actual CPU clock
 * is different.
 */
#define ULTRASONIC_CPU_FREQ_MHZ  25UL

/*
 * Maximum waiting time for ECHO
 * in microseconds.
 */
#define ULTRASONIC_TIMEOUT_US    30000UL

#endif