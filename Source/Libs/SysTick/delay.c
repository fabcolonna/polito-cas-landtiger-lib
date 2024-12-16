#include "delay.h"
#include "system_LPC17xx.h"

_DECL_EXTERNALLY void Tick_Init();
_DECL_EXTERNALLY void Tick_Delay(u32);

void Delay(u32 secs)
{
    Tick_Init();

    const u32 ticks = SystemCoreClock * secs;
    Tick_Delay(ticks);
}