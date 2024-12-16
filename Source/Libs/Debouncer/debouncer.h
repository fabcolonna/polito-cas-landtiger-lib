#ifndef __DEBOUNCER_H
#define __DEBOUNCER_H

#include "utils.h"
#include <stdint.h>

/// @brief Initialize the debouncer
/// @param ival_ms Interval in millis between each debounce check
/// @param int_priority Debouncer interrupt priority (0 (highest), 15 (lowest)). If set to
///        INT_PRIO_DEF, the default priority will be used.
void DEB_Init(u32 ival_ms, u16 int_priority);

void DEB_Enable(void);
void DEB_Disable(void);

#endif