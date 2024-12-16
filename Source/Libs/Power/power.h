#ifndef __POWER_H
#define __POWER_H

#include "power_types.h"
#include "utils.h"
#include <stdbool.h>

/// @brief Configures sleep/power down mode for the device.
/// @param config Settings for the power module (Use DEEP mode, and SLEEPONEXIT)
/// @param sleep_on_exit Whether to enter sleep mode on exit from an ISR
void Power_Init(u8 config);

/// @brief WFI/WFE puts device to sleep or deep sleep mode depending on
///        the configuration specified in the Power_Init function.
void Power_SleepOnWFI(void);

/// @brief WFI/WFE puts device to power down power down mode.
void Power_PowerDownOnWFI(void);

/// @brief WFI/WFE puts device to deep power down mode
///        provided that the SLEEPDEEP bit is set.
void Power_DeepPowerDownOnWFI(void);

/// @brief Turns on a peripheral.
/// @param peripheral The bit # of the peripheral to turn on, defined in the POWER_Peripheral enum. If
///        not present, the bit can be found in the user manual @ Table 46.
void Power_TurnOnPeripheral(u8 bit);

/// @brief Turns off a peripheral.
/// @param peripheral The bit # of the peripheral to turn off, defined in the POWER_Peripheral enum. If
///        not present, the bit can be found in the user manual @ Table 46.
void Power_TurnOffPeripheral(u8 bit);

// WFI function

_INLINE void WFI(void)
{
    __asm volatile("wfi");
}

#endif
