#ifndef __TOUCH_H
#define __TOUCH_H

#include "glcd_types.h"
#include "utils.h"

// TYPES

typedef struct
{
    u16 x, y;
} TP_Coordinate;

// FUNCTIONS

void TP_Init(void);

const TP_Coordinate *TP_WaitForTouch(void);

const LCD_Coordinate *TP_GetLCDCoordinateFor(TP_Coordinate *const tp_point);

#endif