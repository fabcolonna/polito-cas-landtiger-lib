#ifndef BUTTON_H
#define BUTTON_H

#include "button_types.h"
#include "utils.h"

/// @brief Initializes the BUTTON peripherals
/// @param options Button options from the BUTTON_Config enum
/// @param eint0_priority Interrupt priority (0 (highest), 15 (lowest)) for EINT0.
/// @param eint1_priority Interrupt priority for EINT1.
/// @param eint2_priority Interrupt priority for EINT2.
/// @note If INT_DEF_PRIO is passed to the priorities, the default priority will be used.
void BUTTON_Init(u8 options, u8 eint0_priority, u8 eint1_priority, u8 eint2_priority);

void BUTTON_Deinit(void);

// INTERRUPTS

/// @brief Sets the interrupt handler for a TIMER peripheral, on a specific source between
///        the 4 match registers and 2 capture channels (enum TIMER_Interrupt_Source).
/// @param source Button interrupt source (enum BUTTON_Interrupt_Source)
/// @param handler Function pointer to the interrupt handler: void function(void)
void BUTTON_SetInterruptHandler(BUTTON_InterruptSource source, BUTTON_InterruptHandler handler);

/// @brief Unsets the interrupt handler for a TIMER peripheral, on a specific source between
///        the 4 match registers and 2 capture channels (enum TIMER_Interrupt_Source).
/// @param source Interrupt source (enum TIMER_Interrupt_Source)
void BUTTON_UnsetInterruptHandler(BUTTON_InterruptSource source);

#endif