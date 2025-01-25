#ifndef __ADC_PM_TYPES_H
#define __ADC_PM_TYPES_H

#include "types.h"

typedef enum
{
    /// @brief No error occurred.
    ADC_PM_ERR_OK = 0x0,

    /// @brief The ADC was initialized with the SAMPLE_WITH_RIT
    ///        option, but the RIT peripheral was not initialized.
    ADC_PM_RIT_UNINIT,
} ADC_PMError;

/// @brief The type of the function that will be executed when a conversion is done.
typedef void (*ADC_PMInterruptHandler)(u16 prv_value, u16 new_value);

typedef enum
{
    ADC_PM_SAMPLE_WITH_RIT = 0x1,
    ADC_PM_SAMPLE_MANUALLY = 0x2,
} ADC_PMConfig;

#endif