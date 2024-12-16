#ifndef __DEBOUNCER_H
#define __DEBOUNCER_H

#include "utils.h"
#include <stdint.h>

/// @brief Initialize the debouncer
/// @param ival_ms Interval in millis between each debounce check
/// @param pclk Peripheral clock frequency in MHz
/// @param int_priority Debouncer interrupt priority (0 (highest), 15 (lowest)). If set to DEF_PRIORITY,
///        the default priority will be used.
void DEB_Init(u32 ival_ms, u32 pclk_mhz, u16 int_priority);

void DEB_Enable(void);
void DEB_Disable(void);
void DEB_Reset(void);

#endif