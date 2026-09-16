/*
 * SYSCFG_prv.h
 *
 *  Created on: 24 Aug 2026
 *      Author: LOQ
 */

#ifndef MCAL_AFIO_AFIO_PRV_H_
#define MCAL_AFIO_AFIO_PRV_H_


#define SYSCFG_BASE_ADDRESS  0x40010000U

typedef struct{
	u32 EVCR;
	u32 MAPR;
	u32 EXTICRx[4] ;
	u32 RESERVED;
	u32 MAPR2;

}SYSCFG_MamMap_t;

#define SYSCFG ((volatile SYSCFG_MamMap_t*)(SYSCFG_BASE_ADDRESS))



#endif /* MCAL_AFIO_AFIO_PRV_H_ */
