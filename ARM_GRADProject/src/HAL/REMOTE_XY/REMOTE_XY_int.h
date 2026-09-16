#ifndef REMOTEXY_INT_H_
#define REMOTEXY_INT_H_

#include "../../LIB/STD_TYPES.h"


typedef enum
{
    ROBOT_STOP = 0,
    ROBOT_FORWARD,
    ROBOT_BACKWARD,
    ROBOT_LEFT,
    ROBOT_RIGHT

} RobotDirection_t;


void APP_RemoteXY_vInit(void);

void APP_RemoteXY_vUpdate(void);

RobotDirection_t APP_RemoteXY_u8GetDirection(void);

u8 APP_RemoteXY_u8GetSpeed(void);


#endif
