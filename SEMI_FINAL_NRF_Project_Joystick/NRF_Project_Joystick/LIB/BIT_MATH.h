/************************************************/
/* Author  : Monica                             */
/* Layer   : LIB                                */
/* File    : BIT_MATH.h                         */
/************************************************/
#ifndef BIT_MATH_H
#define BIT_MATH_H

#define SET_BIT(VAR,BIT)   ((VAR) |=  (1u << (BIT)))
#define CLR_BIT(VAR,BIT)   ((VAR) &= ~(1u << (BIT)))
#define TOG_BIT(VAR,BIT)   ((VAR) ^=  (1u << (BIT)))
#define GET_BIT(VAR,BIT)   (((VAR) >> (BIT)) & 1u)

#endif
