#include <LPC17xx.h>
#include <stdlib.h>
#include "timer.h"

/// @brief Array of function pointers to the interrupt handlers for each TIMER peripheral.
///        The array is of size TIM_COUNT * TIM_INT_SRC_COUNT, where TIM_COUNT is 4
///        and TIM_INT_SRC_COUNT is the number of interrupt sources (4 match registers and
///        2 capture channels, although the latter are not implemented in this library).
/// @note This array is updated by TIMER_SetInterruptHandler() and TIMER_UnsetInterruptHandler().
_PRIVATE TIMER_InterruptHandler handlers[TIM_COUNT * TIM_INT_SRC_COUNT] = {NULL};

// PRIVATE FUNCTIONS

/// @brief Checks the TIMER's Interrupt Register and returns whether the interrupt
///        was triggered by the specified source, which is of type TIMER_Interrupt_Source.
/// @param source A TIMER_Interrupt_Source object
/// @note IR has bits [0..3] for the 4 MR available, and 2 bits [4, 5] for the 2 capture
///       channels, that have NOT been implemented in this library yet. Hence, although
///       they've been added to the enum for completeness, they won't work (return false).
_PRIVATE bool who_did_interrupt(u8 which, u8 source)
{
    switch (which)
    {
    case 0:
        return LPC_TIM0->IR & (1 << source);
    case 1:
        return LPC_TIM1->IR & (1 << source);
    case 2:
        return LPC_TIM2->IR & (1 << source);
    case 3:
        return LPC_TIM3->IR & (1 << source);
    }

    return false;
}

/// @brief Clears the interrupt flag for the specified source.
/// @note IR expects a 1 to clear the interrupt flag.
_PRIVATE void clear_interrupt(u8 which, u8 source)
{
    switch (which)
    {
    case 0:
        SET_BIT(LPC_TIM0->IR, source);
        break;
    case 1:
        SET_BIT(LPC_TIM1->IR, source);
        break;
    case 2:
        SET_BIT(LPC_TIM2->IR, source);
        break;
    case 3:
        SET_BIT(LPC_TIM3->IR, source);
        break;
    }
}

// PUBLIC FUNCTIONS

void TIMER_SetInterruptHandler(TIMER timer, u8 source, TIMER_InterruptHandler handler)
{
    handlers[timer.which * TIM_INT_SRC_COUNT + source] = handler;
}

void TIMER_UnsetInterruptHandler(TIMER timer, u8 source)
{
    handlers[timer.which * TIM_INT_SRC_COUNT + source] = NULL;
}

// INTERRUPT HANDLERS.

extern void TIMER0_IRQHandler(void)
{
    if (who_did_interrupt(0, TIM_INT_SRC_MR0))
    {
        clear_interrupt(0, TIM_INT_SRC_MR0);
        if (handlers[0])
            handlers[0]();
    }

    if (who_did_interrupt(0, TIM_INT_SRC_MR1))
    {
        clear_interrupt(0, TIM_INT_SRC_MR1);
        if (handlers[1])
            handlers[1]();
    }

    if (who_did_interrupt(0, TIM_INT_SRC_MR2))
    {
        clear_interrupt(0, TIM_INT_SRC_MR2);
        if (handlers[2])
            handlers[2]();
    }

    if (who_did_interrupt(0, TIM_INT_SRC_MR3))
    {
        clear_interrupt(0, TIM_INT_SRC_MR3);
        if (handlers[3])
            handlers[3]();
    }
}

extern void TIMER1_IRQHandler(void)
{
    if (who_did_interrupt(1, TIM_INT_SRC_MR0))
    {
        clear_interrupt(1, TIM_INT_SRC_MR0);
        if (handlers[4])
            handlers[4]();
    }

    if (who_did_interrupt(1, TIM_INT_SRC_MR1))
    {
        clear_interrupt(1, TIM_INT_SRC_MR1);
        if (handlers[5])
            handlers[5]();
    }

    if (who_did_interrupt(1, TIM_INT_SRC_MR2))
    {
        clear_interrupt(1, TIM_INT_SRC_MR2);
        if (handlers[6])
            handlers[6]();
    }

    if (who_did_interrupt(1, TIM_INT_SRC_MR3))
    {
        clear_interrupt(1, TIM_INT_SRC_MR3);
        if (handlers[7])
            handlers[7]();
    }
}

extern void TIMER2_IRQHandler(void)
{
    if (who_did_interrupt(2, TIM_INT_SRC_MR0))
    {
        clear_interrupt(2, TIM_INT_SRC_MR0);
        if (handlers[8])
            handlers[8]();
    }

    if (who_did_interrupt(2, TIM_INT_SRC_MR1))
    {
        clear_interrupt(2, TIM_INT_SRC_MR1);
        if (handlers[9])
            handlers[9]();
    }

    if (who_did_interrupt(2, TIM_INT_SRC_MR2))
    {
        clear_interrupt(2, TIM_INT_SRC_MR2);
        if (handlers[10])
            handlers[10]();
    }

    if (who_did_interrupt(2, TIM_INT_SRC_MR3))
    {
        clear_interrupt(2, TIM_INT_SRC_MR3);
        if (handlers[11])
            handlers[11]();
    }
}

extern void TIMER3_IRQHandler(void)
{
    if (who_did_interrupt(3, TIM_INT_SRC_MR0))
    {
        clear_interrupt(3, TIM_INT_SRC_MR0);
        if (handlers[12])
            handlers[12]();
    }

    if (who_did_interrupt(3, TIM_INT_SRC_MR1))
    {
        clear_interrupt(3, TIM_INT_SRC_MR1);
        if (handlers[13])
            handlers[13]();
    }

    if (who_did_interrupt(3, TIM_INT_SRC_MR2))
    {
        clear_interrupt(3, TIM_INT_SRC_MR2);
        if (handlers[14])
            handlers[14]();
    }

    if (who_did_interrupt(3, TIM_INT_SRC_MR3))
    {
        clear_interrupt(3, TIM_INT_SRC_MR3);
        if (handlers[15])
            handlers[15]();
    }
}