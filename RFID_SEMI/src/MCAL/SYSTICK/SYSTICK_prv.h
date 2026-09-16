/*
 * SYSTICK_prv.h
 *
 *  Created on: 24 Aug 2026
 *      Author: LOQ
 */

#ifndef MCAL_SYSTICK_SYSTICK_PRV_H_
#define MCAL_SYSTICK_SYSTICK_PRV_H_

#define SYSTICK_BASE_ADDRESS  0xE000E010U

#define ENANBLE  0
#define TICKINT 1
#define CLKSOURCE 2
#define COUNTFLAG 16


typedef struct{
	u32 CTRL;
	u32 LOAD;
	u32 VAL;
	u32 CALIB ;

}SYSTICK_MemMap_t;

#define SYSTICK ((volatile SYSTICK_MemMap_t*)(SYSTICK_BASE_ADDRESS))

#endif /* MCAL_SYSTICK_SYSTICK_PRV_H_ */
