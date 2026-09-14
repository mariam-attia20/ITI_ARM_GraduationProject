/*
 * Flash_prv.h
 *
 *  Created on: 9 Sep 2026
 *      Author: LOQ
 */

#ifndef MCAL_FLASH_FLASH_PRV_H_
#define MCAL_FLASH_FLASH_PRV_H_


#define FLASH_BASE_ADDRESS 0x40022000U

typedef struct{
	u32 ACR;
	u32 KEYR;
	u32 OPTKEYR;
	u32 SR ;
	u32 CR ;
	u32 AR;
	u32 RESERVED1 ;
	u32 OBR;
	u32 WRPR;
}FLASH_MemMap_t;

#define FMI  ((volatile FLASH_MemMap_t*)(FLASH_BASE_ADDRESS))

#define KEY1  0x45670123
#define KEY2  0xCDEF89AB

#define BSY   0

#define STRT  6
#define PER   1
#define LOCK  7
#define PG    0
#define MER   2






#endif /* MCAL_FLASH_FLASH_PRV_H_ */
