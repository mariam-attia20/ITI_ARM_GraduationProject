#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

#include "../MCAL/RCC/RCC_int.h"
#include "../MCAL/GPIO/GPIO_int.h"
#include "../MCAL/Flash/Flash_int.h"
void main(){
	MRCC_vInit();
    MRCC_vEnableClk(RCC_APB2, RCC_GPIOA);
    GPIOx_PinConfig_t LED={
    				.Port = GPIO_PORTA,
    				.Pin = GPIO_PIN0 ,
    				.Mode =  GPIO_MODE_OUTPUT ,
    				.Speed = GPIO_SPEED_LOW ,
    				.OutputType = GPIO_OT_PUSHPULL,
    				.PullType = GPIO_NO_PULL ,
    	    };
    MGPIO_vPinInit(&LED);
    MGPIO_vSetPinValue(GPIO_PORTA, GPIO_PIN0 , GPIO_HIGH);

    while(1)
    {

    }
}
