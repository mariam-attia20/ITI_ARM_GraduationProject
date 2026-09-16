/*
 * LedMatrix.c
 *
 *  Created on: 25 Aug 2026
 *      Author: LOQ
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/GPIO/GPIO_int.h"
#include "../../MCAL/SYSTICK/SYSTICK_int.h"

#include "LedMatrix_prv.h"
#include "LedMatrix_cfg.h"
#include "LedMatrix_int.h"

static void HLEDMATRIX_vEnableCurrentCol(u8 A_u8Cols);
static void HLEDMATRIX_vDisableAllCol();
static void HLEDMATRIX_vSetRowValue(u8 A_u8RowValue);

GPIOx_PinConfig_t *Rows;
GPIOx_PinConfig_t *Cols;

u8 NO_ROWS;
u8 NO_COLS;


void HLEDMATRIX_vInit(GPIOx_PinConfig_t *A_xRows ,u8 A_u8RowsNo , u8 A_u8Cols, GPIOx_PinConfig_t *A_xCols)
{
	Rows = A_xRows ;
	Cols = A_xCols ;
	NO_ROWS = A_u8RowsNo;
	NO_COLS = A_u8Cols ;
	for(int i = 0 ; i< A_u8RowsNo; i++)
	{
		MGPIO_vPinInit(&A_xRows[i]);
	}
	for(int i = 0 ; i< A_u8Cols; i++)
	{
		MGPIO_vPinInit(&Cols[i]);
	}
	MYSYSTICK_CFG_T stk_cfg ={
			.InterruptEnable = INT_DISABLE,
			.CLK_SRC = CLK_SRC_AHB_8
	};

	MSYSTICK_vInit(&stk_cfg);
}

void HLEDMATRIX_vDisplayFrame(u8 A_u8Frame[] , u32 A_u32FrameDelay)
{
	for(u8 j = 0 ; j< A_u32FrameDelay ; j++){
		for(u8 i=0 ; i< NO_COLS; i++)
		{
			//set row value
			HLEDMATRIX_vSetRowValue( A_u8Frame[i]);
			// col enable
			HLEDMATRIX_vEnableCurrentCol(i);
			// dealy calcukations
			MYSYTICK_vSetDelay_ms(SCAN_TIME);
			//disable col
			HLEDMATRIX_vDisableAllCol();

		}
	}
}
static void HLEDMATRIX_vEnableCurrentCol(u8 A_u8Cols)
{
	//reminder try to write set port function with special argument
	MGPIO_vSetPinValue( Cols[A_u8Cols].Port,Cols[A_u8Cols].Pin, GPIO_LOW);
}
static void HLEDMATRIX_vDisableAllCol()
{
	for(u8 i =0 ; i < NO_COLS ; i++)
	{
		MGPIO_vSetPinValue( Cols[i].Port,Cols[i].Pin, GPIO_HIGH);
	}

}
static void HLEDMATRIX_vSetRowValue(u8 A_u8RowValue)
{
	for(u8 i = 0; i < NO_ROWS; i++)
	    {
	        MGPIO_vSetPinValue( Rows[i].Port,Rows[i].Pin,GET_BIT(A_u8RowValue, i));
	    }
}




