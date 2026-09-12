////    /*
////     * IR.c
////     *
////     *  Created on: 1 Sep 2026
////     *      Author: LOQ
////     */
////
//    #include "../LIB/STD_TYPES.h"
//    #include "../LIB/BIT_MATH.h"
//
//    #include "../MCAL/RCC/RCC_int.h"
//    #include "../MCAL/SYSTICK/SYSTICK_int.h"
//    #include "../MCAL/GPIO/GPIO_int.h"
//    #include "../MCAL/UART/UART_int.h"
//    #include "../MCAL/NVIC/NVIC_int.h"
//    #include "../MCAL/EXTI/EXTI_int.h"
//
//
//   void APP_GET_TIME(void);
//   void APP_DecodeBits(void);
//
//   /* Global State Variables */
//   volatile u8  G_u8StartingFlag = 0;
//   volatile u32 G_u32Arr[50]     = {0};
//   volatile u8  G_u8Counter      = 0;
//   volatile u8  G_u8Value        = 0;
//
//   u8 SevenSegment[10] =
//   {
//       0b11000000,   // 0
//       0b11111001,   // 1
//       0b10100100,   // 2
//       0b10110000,   // 3
//       0b10011001,   // 4
//       0b10010010,   // 5
//       0b10000010,   // 6
//       0b11111000,   // 7
//       0b10000000,   // 8
//       0b10010000    // 9
//   };
//
//
//   static void reset(void)
//   {
//  		MGPIO_vSetPinValue(GPIO_PORTA , GPIO_PIN1, GPIO_LOW);
//  		MGPIO_vSetPinValue(GPIO_PORTA , GPIO_PIN2, GPIO_LOW);
//  		MGPIO_vSetPinValue(GPIO_PORTA , GPIO_PIN3, GPIO_LOW);
//  		MGPIO_vSetPinValue(GPIO_PORTA , GPIO_PIN4, GPIO_LOW);
//  		MGPIO_vSetPinValue(GPIO_PORTA , GPIO_PIN5, GPIO_LOW);
//  		MGPIO_vSetPinValue(GPIO_PORTA , GPIO_PIN6, GPIO_LOW);
//  		MGPIO_vSetPinValue(GPIO_PORTA , GPIO_PIN7, GPIO_LOW);
//  		MGPIO_vSetPinValue(GPIO_PORTA , GPIO_PIN8, GPIO_LOW);
//   }
//   static void forward(void)
//   {
//  		reset();
//  		MGPIO_vSetPinValue(GPIO_PORTA , GPIO_PIN1, GPIO_HIGH);
//   }
//   static void backward(void)
//   {
//  		reset();
//  		MGPIO_vSetPinValue(GPIO_PORTA , GPIO_PIN4, GPIO_HIGH);
//   }
//   static void left(void)
//   {
//  		reset();
//  		MGPIO_vSetPinValue(GPIO_PORTA , GPIO_PIN2, GPIO_HIGH);
//   }
//   static void right(void)
//   {
//  		reset();
//  		MGPIO_vSetPinValue(GPIO_PORTA , GPIO_PIN3, GPIO_HIGH);
//   }
//
//   int main(void)
//   {
//       /* 1. Initialize System Clock */
//       MRCC_vInit();
//
//       /* ---------------- Peripheral Clock Selection ---------------- */
//
//       /* Cortex-M3 (STM32F1) - Alternative */
//        MRCC_vEnableClk(RCC_APB2, RCC_GPIOA);
//        MRCC_vEnableClk(RCC_APB2, 0);  // AFIO Clock (Bit 0)
//
//       /* 2. Configure IR Input Pin (PA0) */
//       GPIOx_PinConfig_t IR = {
//           .Port = GPIO_PORTA,
//           .Pin  = GPIO_PIN0,
//           .Mode = GPIO_MODE_INPUT
//       };
//       MGPIO_vPinInit(&IR);
//
//       /* 3. Configure Status LED Pin (PA1) */
//       GPIOx_PinConfig_t SegmentPins[] = {
//           {GPIO_PORTA, GPIO_PIN1, GPIO_MODE_OUTPUT, GPIO_OT_PUSHPULL, GPIO_SPEED_LOW, GPIO_NO_PULL,0},
//           {GPIO_PORTA, GPIO_PIN2, GPIO_MODE_OUTPUT, GPIO_OT_PUSHPULL, GPIO_SPEED_LOW, GPIO_NO_PULL,0},
//           {GPIO_PORTA, GPIO_PIN3, GPIO_MODE_OUTPUT, GPIO_OT_PUSHPULL, GPIO_SPEED_LOW, GPIO_NO_PULL,0},
//           {GPIO_PORTA, GPIO_PIN4, GPIO_MODE_OUTPUT, GPIO_OT_PUSHPULL, GPIO_SPEED_LOW, GPIO_NO_PULL,0},
//           {GPIO_PORTA, GPIO_PIN5, GPIO_MODE_OUTPUT, GPIO_OT_PUSHPULL, GPIO_SPEED_LOW, GPIO_NO_PULL,0},
//           {GPIO_PORTA, GPIO_PIN6, GPIO_MODE_OUTPUT, GPIO_OT_PUSHPULL, GPIO_SPEED_LOW, GPIO_NO_PULL,0},
//		   {GPIO_PORTA, GPIO_PIN7, GPIO_MODE_OUTPUT, GPIO_OT_PUSHPULL, GPIO_SPEED_LOW, GPIO_NO_PULL,0}
//       };
//       for(u8 i = 0; i < 7; i++)
//       {
//           MGPIO_vPinInit(&SegmentPins[i]);
//       }
//
//       /* 4. Configure External Interrupt (EXTI Line 0, Falling Edge) */
//       MEXTI_vInit();
//       MEXTI_vEnableINT(EXTI_LINE0);
//       MEXTI_vSetTrigger(EXTI_LINE0, EXTI_FALLING_EDGE);
//       MEXTI_vSetCallBack(APP_GET_TIME, EXTI_LINE0);
//
//       /* 5. Enable EXTI0 IRQ Channel in NVIC (Position 6) */
//       MNVIC_vEnable_Perpheral_INT(6);
//
//       /* 6. Configure SysTick Timer (Interrupt Enabled, AHB / 8 Clock Source) */
//       MYSYSTICK_CFG_T STK_CFG = {
//           .InterruptEnable = INT_ENABLE,
//           .CLK_SRC         = CLK_SRC_AHB_8
//       };
//       MSYSTICK_vInit(&STK_CFG);
//
//       while(1)
//       {
//       	switch(G_u8Value){
//       	case 24:		//2
//       		forward();
//       		break;
//       	case 8:			//4
//       		left();
//       		break;
//       	case 90:		//6
//       		right();
//       		break;
//       	case 82:		//8
//       		backward();
//       		break;
//       	case 69:		//power off
//       		reset();
//       		break;
//       	}
//       }
//
//       return 0;
//   }
//
//
//   void APP_GET_TIME(void)
//   {
//       if(G_u8StartingFlag == 0)
//       {
//           /* First Falling Edge: Start of Frame Leader Code */
//           G_u8StartingFlag = 1;
//           MYSYTICK_vSetInterval_Single(15, APP_DecodeBits);
//       }
//       else
//       {
//           /* Capture bit pulse duration in microseconds */
//           G_u32Arr[G_u8Counter++] = MSTSTICK_u32GetElapsedTime_SingleShut() / 1;
//
//           /* Rearm timeout for the next incoming bit */
//           MYSYTICK_vSetInterval_Single(4, APP_DecodeBits);
//       }
//   }
//
//   /* ================= SysTick Callback: Frame Decoder ================= */
//
//   void APP_DecodeBits(void)
//   {
//       G_u8Value = 0;
//
//       /* Decode Command Byte (Indices 17 to 24) */
//       for(u8 i = 0; i < 8; i++)
//       {
//           if(G_u32Arr[17 + i] >= 1000 && G_u32Arr[17 + i] <= 1250)
//           {
//               CLR_BIT(G_u8Value, i); // Logical '0'
//           }
//           else if(G_u32Arr[17 + i] >= 2000 && G_u32Arr[17 + i] <= 2450)
//           {
//               SET_BIT(G_u8Value, i); // Logical '1'
//           }
//       }
//
//       /* Reset State Flags and Counters */
//       G_u8StartingFlag = 0;
//       G_u8Counter      = 0;
//
//       /* Clear capture buffer */
//       for(u8 i = 0; i < 50; i++)
//       {
//           G_u32Arr[i] = 0;
//       }
//   }
//
//
//
//
//
//
//
//
//
//
//
