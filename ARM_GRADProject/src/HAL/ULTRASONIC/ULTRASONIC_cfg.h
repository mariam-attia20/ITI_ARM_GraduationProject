/*
 * ULTRASONIC_cfg.h
 */

#ifndef HAL_ULTRASONIC_ULTRASONIC_CFG_H_
#define HAL_ULTRASONIC_ULTRASONIC_CFG_H_

/* Ultrasonic pins */

#define ULTRASONIC_TRIG_PORT     GPIO_PORTB
#define ULTRASONIC_TRIG_PIN      GPIO_PIN8

#define ULTRASONIC_ECHO_PORT     GPIO_PORTB
#define ULTRASONIC_ECHO_PIN      GPIO_PIN9


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