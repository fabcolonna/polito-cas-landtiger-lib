#include "LPC17xx.h"
#include "includes.h"
#include "timer.h"

/// @brief Array of function pointers to the interrupt handlers for each TIMER peripheral.
///        The array is of size TIMERS_COUNT * TIM_INT_SRC_COUNT, where TIMERS_COUNT is 4
///        and TIM_INT_SRC_COUNT is the number of interrupt sources (4 match registers and
///        2 capture channels, although the latter are not implemented in this library).
/// @note This array is updated by TIMER_SetInterruptHandler() and TIMER_UnsetInterruptHandler().
_PRIVATE TIMER_InterruptHandler TIMER_Handlers[TIMERS_COUNT * TIM_INT_SRC_COUNT] = {NULL};

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
    TIMER_Handlers[timer.which * TIM_INT_SRC_COUNT + source] = handler;
}

void TIMER_UnsetInterruptHandler(TIMER timer, u8 source)
{
    TIMER_Handlers[timer.which * TIM_INT_SRC_COUNT + source] = NULL;
}

// INTERRUPT HANDLERS.

_INT_HANDLER TIMER0_IRQHandler(void)
{
    if (who_did_interrupt(0, TIM_INT_SRC_MR0))
    {
        clear_interrupt(0, TIM_INT_SRC_MR0);
        if (TIMER_Handlers[0])
            TIMER_Handlers[0]();
    }

    if (who_did_interrupt(0, TIM_INT_SRC_MR1))
    {
        clear_interrupt(0, TIM_INT_SRC_MR1);
        if (TIMER_Handlers[1])
            TIMER_Handlers[1]();
    }

    if (who_did_interrupt(0, TIM_INT_SRC_MR2))
    {
        clear_interrupt(0, TIM_INT_SRC_MR2);
        if (TIMER_Handlers[2])
            TIMER_Handlers[2]();
    }

    if (who_did_interrupt(0, TIM_INT_SRC_MR3))
    {
        clear_interrupt(0, TIM_INT_SRC_MR3);
        if (TIMER_Handlers[3])
            TIMER_Handlers[3]();
    }
}

_INT_HANDLER TIMER1_IRQHandler(void)
{
    if (who_did_interrupt(1, TIM_INT_SRC_MR0))
    {
        clear_interrupt(1, TIM_INT_SRC_MR0);
        if (TIMER_Handlers[4])
            TIMER_Handlers[4]();
    }

    if (who_did_interrupt(1, TIM_INT_SRC_MR1))
    {
        clear_interrupt(1, TIM_INT_SRC_MR1);
        if (TIMER_Handlers[5])
            TIMER_Handlers[5]();
    }

    if (who_did_interrupt(1, TIM_INT_SRC_MR2))
    {
        clear_interrupt(1, TIM_INT_SRC_MR2);
        if (TIMER_Handlers[6])
            TIMER_Handlers[6]();
    }

    if (who_did_interrupt(1, TIM_INT_SRC_MR3))
    {
        clear_interrupt(1, TIM_INT_SRC_MR3);
        if (TIMER_Handlers[7])
            TIMER_Handlers[7]();
    }
}

_INT_HANDLER TIMER2_IRQHandler(void)
{
    if (who_did_interrupt(2, TIM_INT_SRC_MR0))
    {
        clear_interrupt(2, TIM_INT_SRC_MR0);
        if (TIMER_Handlers[8])
            TIMER_Handlers[8]();
    }

    if (who_did_interrupt(2, TIM_INT_SRC_MR1))
    {
        clear_interrupt(2, TIM_INT_SRC_MR1);
        if (TIMER_Handlers[9])
            TIMER_Handlers[9]();
    }

    if (who_did_interrupt(2, TIM_INT_SRC_MR2))
    {
        clear_interrupt(2, TIM_INT_SRC_MR2);
        if (TIMER_Handlers[10])
            TIMER_Handlers[10]();
    }

    if (who_did_interrupt(2, TIM_INT_SRC_MR3))
    {
        clear_interrupt(2, TIM_INT_SRC_MR3);
        if (TIMER_Handlers[11])
            TIMER_Handlers[11]();
    }
}

_INT_HANDLER TIMER3_IRQHandler(void)
{
    if (who_did_interrupt(3, TIM_INT_SRC_MR0))
    {
        clear_interrupt(3, TIM_INT_SRC_MR0);
        if (TIMER_Handlers[12])
            TIMER_Handlers[12]();
    }

    if (who_did_interrupt(3, TIM_INT_SRC_MR1))
    {
        clear_interrupt(3, TIM_INT_SRC_MR1);
        if (TIMER_Handlers[13])
            TIMER_Handlers[13]();
    }

    if (who_did_interrupt(3, TIM_INT_SRC_MR2))
    {
        clear_interrupt(3, TIM_INT_SRC_MR2);
        if (TIMER_Handlers[14])
            TIMER_Handlers[14]();
    }

    if (who_did_interrupt(3, TIM_INT_SRC_MR3))
    {
        clear_interrupt(3, TIM_INT_SRC_MR3);
        if (TIMER_Handlers[15])
            TIMER_Handlers[15]();
    }
}