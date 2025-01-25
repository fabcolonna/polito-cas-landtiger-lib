#ifndef __TIMER_H
#define __TIMER_H

#include "timer_types.h"
#include "types.h"
#include <stdbool.h>
#include <stdint.h>

/// @brief Initializes a TIMER peripheral.
/// @param timer [OUTPUT] Configured TIMER peripheral
/// @param which Which timer to initialize (0-3)
/// @param prescaler Prescaler value. If set to NO_PRESCALER, prescaler won't be used.
/// @param int_priority Timer interrupt priority (0 (highest), 15 (lowest)). If set to
///                     INT_PRIO_DEF, the default priority is set.
void TIMER_Init(TIMER *timer, u8 which, u32 prescaler, u8 int_priority);

/// @brief Deconfigures a TIMER peripheral (also match registers).
/// @note In order to use the TIMER peripheral again, it must be initialized.
void TIMER_Deinit(TIMER timer);

/// @brief  Sets the match value for a match register of a TIMER peripheral
/// @param match_reg Match register to set
void TIMER_SetMatch(TIMER timer, TIMER_MatchRegister match_reg);

/// @brief Unsets a match register of a TIMER peripheral
/// @param match_reg Match register to unset. This variable won't be touched.
void TIMER_UnsetMatch(TIMER timer, TIMER_MatchRegister match_reg);

void TIMER_Enable(TIMER timer);
void TIMER_Disable(TIMER timer);

bool TIMER_IsEnabled(TIMER timer);

/// @brief Resets a TIMER peripheral without deconfiguring it.
/// @note In order to use the TIMER peripheral again, it must be enabled.
void TIMER_Reset(TIMER timer);

// INTERRUPTS

/// @brief Sets the interrupt handler for a TIMER peripheral, on a specific source between
///        the 4 match registers and 2 capture channels (enum TIMER_InterruptSource).
/// @param timer TIMER peripheral
/// @param source Interrupt source (enum TIMER_InterruptSource)
/// @param handler Function pointer to the interrupt handler: void function(void)
void TIMER_SetInterruptHandler(TIMER timer, u8 source, TIMER_InterruptHandler handler);

/// @brief Unsets the interrupt handler for a TIMER peripheral, on a specific source between
///        the 4 match registers and 2 capture channels (enum TIMER_InterruptSource).
/// @param timer TIMER peripheral
/// @param source Interrupt source (enum TIMER_InterruptSource)
void TIMER_UnsetInterruptHandler(TIMER timer, u8 source);

#endif
