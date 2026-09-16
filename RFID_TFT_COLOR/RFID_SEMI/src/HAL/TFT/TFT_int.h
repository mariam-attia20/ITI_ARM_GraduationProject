/*
 * TFT_int.h
 *
 *  Created on: Sep 3, 2026
 *      Author: lenovo
 */

#ifndef HAL_TFT_TFT_INT_H_
#define HAL_TFT_TFT_INT_H_


void HTFT_vInit(void);
void HTFT_vShowImage(const u16 A_u16ImgArr[] , u16 A_u16ImgSize);

void HTFT_vSetXPos(u16 A_u16xStart, u16 A_u16xEnd);
void HTFT_vSetYPos(u16 A_u16yStart, u16 A_u16yEnd);

void HTFT_vFillBackgroundColor(u16 A_u16Color);
void HTFT_vFillRectangle(u16 A_u16Color);

#define Red 	5
#define Green 	6
#define Blue	5
#define Black	0x0000
#define White	0xFFFF

/* Proper RGB565 color values (Red/Green/Blue above are bit-widths, not
 * colors, so they can't be passed to HTFT_vFillBackgroundColor - use
 * these instead). Format is RRRRR GGGGGG BBBBB. */
#define TFT_COLOR_BLACK		0x0000
#define TFT_COLOR_WHITE		0xFFFF
#define TFT_COLOR_RED		0xF800
#define TFT_COLOR_GREEN		0x07E0
#define TFT_COLOR_BLUE		0x001F
#define TFT_COLOR_YELLOW	0xFFE0

#endif /* HAL_TFT_TFT_INT_H_ */
