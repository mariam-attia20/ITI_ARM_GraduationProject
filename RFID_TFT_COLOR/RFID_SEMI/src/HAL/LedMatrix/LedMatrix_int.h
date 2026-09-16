/*
 * LedMatrix_int.h
 *
 *  Created on: 25 Aug 2026
 *      Author: LOQ
 */

#ifndef HAL_LEDMATRIX_LEDMATRIX_INT_H_
#define HAL_LEDMATRIX_LEDMATRIX_INT_H_

void HLEDMATRIX_vInit(GPIOx_PinConfig_t *A_xRows ,u8 A_u8RowsNo , u8 A_u8Cols, GPIOx_PinConfig_t *A_xCols);

void HLEDMATRIX_vDisplayFrame(u8 A_u8Frame[] , u32 A_u32FrameDelay);


#endif /* HAL_LEDMATRIX_LEDMATRIX_INT_H_ */
