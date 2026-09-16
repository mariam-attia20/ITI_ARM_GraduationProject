/*
 * RTOS_int.h
 *
 *  Created on: 26 Aug 2026
 *      Author: LOQ
 */

#ifndef RTOS_RTOS_INT_H_
#define RTOS_RTOS_INT_H_


void OS_vStart(void);
u8 OS_vCreateTask( void(*TaskFunction)(void), u32 Periodicity , u8 Priority );

void OS_vScheduler(void);

//suspend task
u8 OS_vSuspendTask(u8 Priority );

//resume task
u8 OS_vResumeTask(u8 Priority );

//delet task
u8 OS_vDeletTask(u8 Priority );

/* ERROR */
//1 -> RESERVED_PRIORITY_ERRO

#endif /* RTOS_RTOS_INT_H_ */
