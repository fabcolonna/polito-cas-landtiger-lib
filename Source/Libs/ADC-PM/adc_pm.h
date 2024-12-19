#ifndef __ADC_PM_H
#define __ADC_PM_H

#include "utils.h"

/// @brief The type of the function that will be executed when a conversion is done.
typedef void (*ADC_InterruptHandler)(u16 prv_value, u16 new_value);

/// @brief Initializes the 12bit ADC peripheral, which on this board is wired to the Potentiometer (PM).
///        More specifically, the ADC peripheral is wired to the channel 5 of the ADC. Hence, the
///        library does not handle other channels.
/// @param clock_divider The clock divider to be used by the ADC (division is performed by clock_divider+1)
/// @param int_priority The priority of the ADC interrupt. If the priority is set to INT_PRIO_DEF,
///                     the default priority will be used.
void ADC_PMInit(u8 clock_divider, u8 int_priority);

void ADC_PMDeinit(void);

/// @brief Retrieves a single sample from the PM, and triggers the interrupt.
/// @note This function does NOT repetitively sample the PM. It only retrieves a single sample.
///       This is because we are not handling the BURST mode of the ADC. In order to keep retrieving
///       samples, the function must be called repeatedly.
/// @note As of now, only the basic conversion (001) is implemented.
void ADC_PMGetSample(void);

void ADC_PMDisableSampling();

// INTERRUPTS

/// @brief Sets the action to be executed when the conversion is done.
/// @param action The action to be executed when the conversion is done.
void ADC_PMSetSampleReadyAction(ADC_InterruptHandler action);

void ADC_PMUnsetSampleReadyAction(void);

#endif