/*
 * SYSTICK_prg.c
 *
 *  Created on: 24 Aug 2026
 *      Author: LOQ
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "SYSTICK_prv.h"
#include "SYSTICK_int.h"
#include "SYSTICK_cfg.h"

static void(*G_xFptr)(void)= NULL;
u8 G_u8SingleFlag =0 ;
void MSYSTICK_vInit(MYSYSTICK_CFG_T * A_xCfg)
{
	CLR_BIT(SYSTICK->CTRL ,ENANBLE);
	if(A_xCfg->InterruptEnable == INT_ENABLE){
          SET_BIT(SYSTICK->CTRL , TICKINT);
	}
	else if(A_xCfg->InterruptEnable == INT_DISABLE)
	{
		CLR_BIT(SYSTICK->CTRL , TICKINT);
	}

	if(A_xCfg->CLK_SRC == CLK_SRC_AHB)
	{
		  SET_BIT(SYSTICK->CTRL , CLKSOURCE);
	}
	else if(A_xCfg->CLK_SRC == CLK_SRC_AHB_8 )
	{
		CLR_BIT(SYSTICK->CTRL , CLKSOURCE);
	}

}


void MSYSTICK_vStartTimer(u32 A_u32LoadVale)
{
	SYSTICK -> LOAD  = A_u32LoadVale ;
	SYSTICK-> VAL = 0;
	SET_BIT(SYSTICK -> CTRL , ENANBLE);

}
void MSYSTICK_vStopTimer(void)
{
	SYSTICK-> VAL = 0;
	CLR_BIT(SYSTICK -> CTRL , ENANBLE);

}

u32 MSTSTICK_u32GetElapsedTime_SingleShut(void)
{
	u32 ElapsedTime = (SYSTICK-> LOAD)-(SYSTICK-> VAL);
	return ElapsedTime;

}
u32 MSTSTICK_u32GetRemainingTime_SingleShut(void)
{
	u32 RemainingTime = SYSTICK-> VAL ;
	return RemainingTime ;

}
void MYSYTICK_vSetDelay_ms(f64 A_f64Delay_ms)
{
	//8M  AHB/8
	// CLOCK SOURCE 8/8= 1
	//1000 ms
	u32 L_u32Ticks = (u32) (A_f64Delay_ms * 1000.0);
	//reset
	SYSTICK-> VAL = 0;
	if((L_u32Ticks >= 0x00000001) && (L_u32Ticks <0x00FFFFFF))
	{
		MSYSTICK_vStartTimer(L_u32Ticks);
		while(!(GET_BIT(SYSTICK->CTRL , COUNTFLAG)));
		MSYSTICK_vStopTimer();

	}
}

void MYSYTICK_vSetDelay_us(f64 A_f64Delay_us)
{
	//8M  AHB/8
	// CLOCK SOURCE 8/8= 1
	//1000 ms
	u32 L_u32Ticks = (u32) (A_f64Delay_us * 0.5);
	//reset
	SYSTICK-> VAL = 0;
	if((L_u32Ticks >= 0x00000001) && (L_u32Ticks < 0x0FFFFFFF))
	{
		MSYSTICK_vStartTimer(L_u32Ticks);
		while(!(GET_BIT(SYSTICK->CTRL , COUNTFLAG)));
		MSYSTICK_vStopTimer();

	}
}

void MYSYTICK_vSetInterval_Single(u32 A_u32Delay_ms ,void(*Fptr)(void))
{
	G_u8SingleFlag =1 ;
	u32 L_u32Ticks = (u32) (A_u32Delay_ms * 1000);
	G_xFptr = Fptr;
	SYSTICK ->VAL= 0;
	if((L_u32Ticks >= 0x00000001) && (L_u32Ticks < 0x0FFFFFFF))
		{
			MSYSTICK_vStartTimer(L_u32Ticks);
		}


}

void MSYSTICK_vSetInterval_Multi(u32 A_u32Delay_ms , void(*Fptr)(void))
{
	G_u8SingleFlag =0 ;
	u32 L_u32Ticks = (u32) (A_u32Delay_ms * 1000);
		G_xFptr = Fptr;
		SYSTICK ->VAL= 0;
		if((L_u32Ticks >= 0x00000001) && (L_u32Ticks < 0x0FFFFFFF))
			{
				MSYSTICK_vStartTimer(L_u32Ticks);
			}

}



void SysTick_Handler(void)
{
	if(G_xFptr !=NULL)
	{
		G_xFptr();

	}
	if(G_u8SingleFlag == 1 )
	{
		MSYSTICK_vStopTimer();
	}


}
