/*
 * RTOS_prv.h
 *
 *  Created on: 26 Aug 2026
 *      Author: LOQ
 */

#ifndef RTOS_RTOS_PRV_H_
#define RTOS_RTOS_PRV_H_

typedef enum{
	READY = 1,
	RUNNING,
	SUSPENDED
}Task_States_t;

typedef struct{
	void (*TaskFunction)(void);
	u32 Periodicity;  //to be reviewed
	Task_States_t states;
}Task_t;

#define RESERVED_PRIORITY_ERROR 1

#define OUT_OF_SYSTASK_RANGE_ERROR 2

#define NOT_DELATED  3

#endif /* RTOS_RTOS_PRV_H_ */
