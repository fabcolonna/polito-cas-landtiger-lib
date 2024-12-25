#ifndef __ADC_PM_TYPES_H
#define __ADC_PM_TYPES_H

#include "utils.h"

/// @brief The type of the function that will be executed when a conversion is done.
typedef void (*ADC_PMInterruptHandler)(u16 prv_value, u16 new_value);

typedef enum
{
    ADC_PM_SAMPLE_WITH_RIT = 0x1,
    ADC_PM_SAMPLE_MANUALLY = 0x2,
} ADC_PMConfig;

#endif