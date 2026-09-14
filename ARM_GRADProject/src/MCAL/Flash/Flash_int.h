/*
 * Flash_int.h
 *
 *  Created on: 9 Sep 2026
 *      Author: LOQ
 */

#ifndef MCAL_FLASH_FLASH_INT_H_
#define MCAL_FLASH_FLASH_INT_H_


void MFMI_vMassErase(void);

void MFMI_vPageErase(u32 A_u32PageAddress);

void MFMI_vProgramFlash(u32 A_u32Address , u16 *A_PtrData , u16 DataLength);


#endif /* MCAL_FLASH_FLASH_INT_H_ */
