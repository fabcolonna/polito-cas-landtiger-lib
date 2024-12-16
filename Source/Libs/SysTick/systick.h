#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "utils.h"

_DECL_EXTERNALLY void Tick_Init();
_DECL_EXTERNALLY void Tick_Delay(u32);

_INLINE void SYSTICK_Init()
{
    Tick_Init();
}

_INLINE void SYSTICK_DelayMs(u32 ms)
{
    Tick_Delay(ms);
}

#endif