#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../HAL/blutooth/blutooth_int.h"

#include "REMOTE_XY_int.h"


static s8 G_s8JoystickX = 0;
static s8 G_s8JoystickY = 0;

static u8 G_u8Speed = 0;

static u8 G_u8Stop = 1;


void APP_RemoteXY_vInit(void)
{
    G_s8JoystickX = 0;

    G_s8JoystickY = 0;

    G_u8Speed = 0;

    G_u8Stop = 1;
}


void APP_RemoteXY_vUpdate(void)
{
    /*
     * RemoteXY generated communication code
     * must be called here.
     *
     * This is GUI/protocol dependent.
     */
}


RobotDirection_t APP_RemoteXY_u8GetDirection(void)
{
    if(G_u8Stop == 1)
    {
        return ROBOT_STOP;
    }


    if(G_s8JoystickY > 50)
    {
        return ROBOT_FORWARD;
    }


    if(G_s8JoystickY < -50)
    {
        return ROBOT_BACKWARD;
    }


    if(G_s8JoystickX > 50)
    {
        return ROBOT_RIGHT;
    }


    if(G_s8JoystickX < -50)
    {
        return ROBOT_LEFT;
    }


    return ROBOT_STOP;
}


u8 APP_RemoteXY_u8GetSpeed(void)
{
    return G_u8Speed;
}
