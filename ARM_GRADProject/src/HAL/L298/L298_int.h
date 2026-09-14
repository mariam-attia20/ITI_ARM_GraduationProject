/*
 * L298_int.h
 *
 *  Created on: 7 Sep 2026
 *      Author: LOQ
 */

#ifndef HAL_L298_L298_INT_H_
#define HAL_L298_L298_INT_H_

#define MOVE_FORWARD   0
#define MOVE_BACKWARD  1
#define MOVE_LEFT      2
#define MOVE_RIGHT     3
#define MOVE_STOP      4

void HL298_vInit(void);
void HL298_vMove(u8 A_u8Direction, u8 A_u8Speed);
void HL298_vMoveForward(void);
void HL298_vStop(void);




#endif /* HAL_L298_L298_INT_H_ */
