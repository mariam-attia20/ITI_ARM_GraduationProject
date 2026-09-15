/************************************************/
/* Layer   : MCAL                               */
/* SWC     : ADC1 (single conversion, polling)  */
/* File    : ADC_interface.h                    */
/************************************************/
#ifndef ADC_INTERFACE_H
#define ADC_INTERFACE_H

/*
 * ADC1 channel -> pin map on the STM32F103 (LQFP48 / Blue Pill).
 * These are the ONLY pins that can be used as analog inputs.
 * PB6 / PB7 have no ADC function at all.
 */
#define ADC1_CH0     0   /* PA0 */
#define ADC1_CH1     1   /* PA1 */
#define ADC1_CH2     2   /* PA2 */
#define ADC1_CH3     3   /* PA3 */
#define ADC1_CH4     4   /* PA4 */
#define ADC1_CH5     5   /* PA5 */
#define ADC1_CH6     6   /* PA6 */
#define ADC1_CH7     7   /* PA7 */
#define ADC1_CH8     8   /* PB0 */
#define ADC1_CH9     9   /* PB1 */

/* Powers up ADC1, sets the sample time and runs the self calibration.
 * Must be called after MRCC_voidInitSysClock() and MSTK_voidInit().
 * The pin itself still has to be put in GPIO_INPUT_ANALOG mode by the caller. */
void MADC1_voidInit(void);

/* Runs one conversion on the given channel and returns the 12-bit result
 * (0 .. 4095). Blocking, about 63 us with the 239.5-cycle sample time. */
u16  MADC1_u16ReadChannel(u8 Copy_u8Channel);

#endif
