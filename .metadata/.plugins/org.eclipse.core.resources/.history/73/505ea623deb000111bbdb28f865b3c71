/*
 * L298_int.h
 *
 *  Created on: 14 Sep 2026
 *      Author: LOQ
 */

#ifndef DIVERS_HAL_L298_L298_INT_H_
#define DIVERS_HAL_L298_L298_INT_H_

#include "../../LIB/STD_TYPES.h"


#define MOVE_FORWARD    0
#define MOVE_BACKWARD   1
#define MOVE_LEFT       2
#define MOVE_RIGHT      3
#define MOVE_STOP       4


#define MOTOR1          0
#define MOTOR2          1
#define MOTOR3          2
#define MOTOR4          3


void L298_vInit(void);

void L298_vMove(u8 Copy_u8Direction,
                u8 Copy_u8Speed);

void L298_vSetMotorSpeed(u8 Copy_u8Motor,
                         u8 Copy_u8Speed);

void L298_vStop(void);

void L298_vMoveForward(void);


#endif
