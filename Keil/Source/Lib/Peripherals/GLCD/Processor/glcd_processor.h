#ifndef __GLCD_PROC_H
#define __GLCD_PROC_H

#include "glcd_types.h"
#include "types.h"

// TYPES

typedef enum
{
    MD_DRAW_BBOX = 0x1,
    MD_DELETE = 0x2,
    MD_BBOX = 0x4,
    MD_DELETE_FAST = 0x8,
} __ProcessingMode;

// PUBLIC FUNCTIONS

bool __LCD_PROC_DoProcessObject(const LCD_Obj *const obj, LCD_BBox *out_bbox, u8 mode);

#endif