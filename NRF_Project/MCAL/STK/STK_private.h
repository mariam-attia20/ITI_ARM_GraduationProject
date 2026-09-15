#ifndef STK_PRIVATE_H
#define STK_PRIVATE_H

#define STK_BASE_ADDRESS   0xE000E010UL

typedef struct
{
    volatile u32 CTRL;
    volatile u32 LOAD;
    volatile u32 VAL;
    volatile u32 CALIB;
} STK_t;

#define STK   ((STK_t*)STK_BASE_ADDRESS)

/* SysTick is clocked from AHB/8 */
#define STK_TICKS_PER_US   ((F_CPU / 8UL) / 1000000UL)

#endif
