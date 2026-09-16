/*
 * DAC_int.h
 *
 *  Created on: 30 Aug 2026
 *      Author: LOQ
 */

#ifndef HAL_DAC_DAC_INT_H_
#define HAL_DAC_DAC_INT_H_

#include "../../MCAL/GPIO/GPIO_int.h"

void HDAC_vInit(GPIOx_PinConfig_t *A_xPins , u8 A_u8PinsNo);

void HDAC_vSendSample(const u8 *A_u8Ptr , u32 A_u32Index);


#endif /* HAL_DAC_DAC_INT_H_ */
