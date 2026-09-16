/*
 * SYS_CFG_prg.c
 *
 *  Created on: 24 Aug 2026
 *      Author: LOQ
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "AFIO_int.h"
#include "AFIO_prv.h"

 void MSYSCFG_vSetLinePort(u8 A_lineNo , u8 A_PortNo)
 {
	 u8 Reg_no = A_lineNo/4 ;
	 u8 shift_value = (A_lineNo%4)*4 ;

	 SYSCFG-> EXTICRx[Reg_no] = ~(0b1111<< shift_value ); // clear port
	 SYSCFG-> EXTICRx[Reg_no] = A_PortNo << shift_value ;
 }




