#ifndef __RIT_H
#define __RIT_H

#include "utils.h"
#include <stdint.h>

/// @brief Initialize the debouncer
/// @param ival_ms Interval in millis between each debounce check
/// @param int_priority Debouncer interrupt priority (0 (highest), 15 (lowest)). If set to
///        INT_PRIO_DEF, the default priority will be used.
void RIT_Init(u32 ival_ms, u16 int_priority);

void RIT_Enable(void);
void RIT_Disable(void);

#endif