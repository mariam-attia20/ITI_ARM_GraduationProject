/*
 * S2P_int.h
 *
 *  Created on: 30 Aug 2026
 *      Author: LOQ
 */

#ifndef HAL_S2P_S2P_INT_H_
#define HAL_S2P_S2P_INT_H_

typedef struct{

	u8 DataPort;
	u8 DataPin ;

	u8 ShiftCLKPort;
	u8 ShiftCLKPin;

	u8 LatchCLKPort;
	u8 LatchCLKPin;

}S2P_Init_t;

void HS2P_vInit(S2P_Init_t * A_xInit);

void HS2P_vSendData(S2P_Init_t * A_xInit , u32 A_u32Byte);
void HS2P_vSendData(S2P_Init_t * A_xInit , u32 A_u32Byte);
static void HS2P_vLatchDate(S2P_Init_t* A_xInit);
static void HS2P_vShiftDate(S2P_Init_t* A_xInit);


#endif /* HAL_S2P_S2P_INT_H_ */
