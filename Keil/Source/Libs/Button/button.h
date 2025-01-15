#ifndef BUTTON_H
#define BUTTON_H

#include "button_types.h"
#include "utils.h"

/// @brief Initializes the BUTTON peripherals
/// @param options Button options from the BUTTON_Config enum
/// @return BUTTON_Error The error.
BUTTON_Error BUTTON_Init(u8 options);

/// @brief Deinitializes the buttons, by removing all jobs, disabling
///        the interrupts, and removing the debouncing job from the RIT.
void BUTTON_Deinit(void);

// INTERRUPTS

/// @brief Enables the interrupt generation of the given source, provided that
///        there's a function associated to it.
/// @param source The source to enable.
/// @param int_priority The priority to assign to the specified source.
BUTTON_Error BUTTON_EnableSource(BUTTON_Source source, u8 int_priority);

/// @brief Disables the interrupt generation of the given source, but it leaves
///        the function binding.
/// @param source THe source to disable.
void BUTTON_DisableSource(BUTTON_Source source);

/// @brief Binds a given interrupt to a functionality.
/// @param source Button interrupt source (enum BUTTON_Source)
/// @param handler Function pointer to the interrupt handler.
/// @param int_priority The priority to assign to the interrupts coming from
/// the selected source.
/// @param enable Whether to enable the function binding right away.
void BUTTON_SetFunction(BUTTON_Source source, BUTTON_Function func);

/// @brief Unbinds the previously set function from the selected interrupt source.
/// @param source Interrupt source (enum BUTTON_Source)
void BUTTON_UnsetFunction(BUTTON_Source source);

#endif