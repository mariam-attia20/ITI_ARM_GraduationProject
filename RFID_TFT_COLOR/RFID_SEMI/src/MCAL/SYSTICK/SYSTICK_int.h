/*
 * SYSTICK_int.h
 *
 *  Created on: 24 Aug 2026
 *      Author: LOQ
 */

#ifndef MCAL_SYSTICK_SYSTICK_INT_H_
#define MCAL_SYSTICK_SYSTICK_INT_H_

//clck source /interrupt or not

typedef struct{
	u8 InterruptEnable;
	u8 CLK_SRC ;
}MYSYSTICK_CFG_T;

void MSYSTICK_vInit(MYSYSTICK_CFG_T * A_xCfg);
void MSYSTICK_vStartTimer(u32 A_u32LoadVale);
void MSYSTICK_vStopTimer(void);

u32 MSTSTICK_u32GetElapsedTime_SingleShut(void);
u32 MSTSTICK_u32GetRemainingTime_SingleShut(void);

void MYSYTICK_vSetDelay_ms(f64 A_f64Delay_ms);
void MYSYTICK_vSetDelay_us(f64 A_f64Delay_us);


void MYSYTICK_vSetInterval_Single(u32 A_f64Delay_ms ,void(*A_xFptr)(void));

void MSYSTICK_vSetInterval_Multi(u32 A_u32Delay_ms , void(*A_xFptr)(void));


#define INT_ENABLE 1
#define INT_DISABLE 0

#define CLK_SRC_AHB 1
#define CLK_SRC_AHB_8 2

#endif /* MCAL_SYSTICK_SYSTICK_INT_H_ */
