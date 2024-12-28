#include "delay.h"

#include <LPC17xx.h>

/// @brief Ticks for a given number of milliseconds, and returns when done.
_DECL_EXTERNALLY void Tick_Load(u32);
_DECL_EXTERNALLY void Tick_Count(void);
_DECL_EXTERNALLY void Tick_Stop(void);

_DECL_EXTERNALLY u32 SystemFrequency;

void Delay(u32 secs)
{
    u32 ms_to_wait = secs * 1000;
    const u32 one_hundred_ms = SystemFrequency / 10; // Fits in 24 bits if the system frequency is 100MHz

    Tick_Stop();
    Tick_Load(one_hundred_ms);
    do
    {
        Tick_Count();
        ms_to_wait -= 100;
    } while (ms_to_wait > 0);

    Tick_Stop();
}