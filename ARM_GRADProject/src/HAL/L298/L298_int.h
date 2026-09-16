#ifndef DIVERS_HAL_L298_L298_INT_H_
#define DIVERS_HAL_L298_L298_INT_H_

#include "../../LIB/STD_TYPES.h"


#define MOVE_FORWARD     0
#define MOVE_BACKWARD    1
#define MOVE_LEFT        2
#define MOVE_RIGHT       3
#define MOVE_STOP        4


#define MOTOR1           0
#define MOTOR2           1
#define MOTOR3           2
#define MOTOR4           3


void HL298_vInit(void);

void HL298_vMove(u8 A_u8Direction, u8 A_u8Speed);

void HL298_vSetMotorSpeed(u8 A_u8Motor, u8 A_u8Speed);

void HL298_vStop(void);

void HL298_vMoveForward(u8 A_u8Speed);

void HL298_vMoveBackward(u8 A_u8Speed);

void HL298_vMoveLeft(u8 A_u8Speed);

void HL298_vMoveRight(u8 A_u8Speed);


#endif /* DIVERS_HAL_L298_L298_INT_H_ */
