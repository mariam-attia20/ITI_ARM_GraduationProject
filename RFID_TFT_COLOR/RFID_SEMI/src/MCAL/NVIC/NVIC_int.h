/*
 * NVIC_int.h
 *
 *  Created on: 22 Aug 2026
 *      Author: LOQ
 */

#ifndef MCAL_NVIC_NVIC_INT_H_
#define MCAL_NVIC_NVIC_INT_H_

typedef enum{

	Group16Sub0 = 3 ,
	Group8Sub2,
	Group4Sub4,
	Group2Sub8,
	Group0Sub16
}NVIC_Group_t;


void MNVIC_vEnable_Perpheral_INT(u8 A_U8Position);

void MNVIC_vDisable_Perpheral_INT(u8 A_U8Position);
void MNVIC_vSetPendingFlag(u8 A_U8Position) ;

void MNVIC_vCLRPendingFlag(u8 A_U8Position) ;
u8  MNVIC_vGetPendingFlag(u8 A_U8Position) ;

void MNVIC_SetGroupPriority(NVIC_Group_t A_xGroupPriority);

void MNVIC_SetPeriphralPriority( u8 A_U8Position , u8 A_u8Group, u8 A_u8SubGroup);


#endif /* MCAL_NVIC_NVIC_INT_H_ */
