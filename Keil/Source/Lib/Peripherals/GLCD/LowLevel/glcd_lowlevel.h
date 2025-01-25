#ifndef __GLCD_LL_H
#define __GLCD_LL_H

#include "types.h"

// This code handles the low-level operations of the GLCD. It's 
// private to the GLCD library and should not be used by the user. 

void __LCD_LL_Init(u16 orientation);

u16 __LCD_LL_GetPointColor(u16 x, u16 y);

void __LCD_LL_SetPointColor(u16 rgb565, u16 x, u16 y);

void __LCD_LL_FillScreen(u16 color);

#endif