/*
 * Flash_prg.c
 *
 *  Created on: 9 Sep 2026
 *      Author: LOQ
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"\

#include "Flash_int.h"
#include "Flash_prv.h"

void MFMI_vMassErase(void)
{
	while(GET_BIT(FMI->SR ,BSY ));
	if(GET_BIT(FMI->CR , LOCK))
	{
      FMI->KEYR = KEY1;
      FMI->KEYR = KEY2;
	}
	SET_BIT(FMI->CR , MER);
	SET_BIT(FMI->CR , STRT);

	while(GET_BIT(FMI->SR ,BSY ));

	CLR_BIT(FMI->CR , MER);


}

void MFMI_vPageErase(u32 A_u32PageAddress)
{
	while(GET_BIT(FMI->SR ,BSY ));
	if(GET_BIT(FMI->CR , LOCK))
	{
	  FMI->KEYR = KEY1;
	  FMI->KEYR = KEY2;
	}

	SET_BIT(FMI->CR , PER);

	FMI->AR = A_u32PageAddress ;

	SET_BIT(FMI->CR , STRT);

	while(GET_BIT(FMI->SR ,BSY ));

	CLR_BIT(FMI->CR , PER);

}

void MFMI_vProgramFlash(u32 A_u32Address , u16 *A_PtrData , u16 DataLength)
{
	    while(GET_BIT(FMI->SR , BSY ));

		if(GET_BIT(FMI->CR , LOCK))
		{
		  FMI->KEYR = KEY1;
		  FMI->KEYR = KEY2;
		}
		SET_BIT(FMI->CR , PG);

		for(u16 i =0 ; i< DataLength ; i ++)
		{
			*(volatile u16*)A_u32Address = A_PtrData[i];
			A_u32Address+=2;

			while(GET_BIT(FMI->SR ,BSY ));
		}

		CLR_BIT(FMI->CR , PG);

}
