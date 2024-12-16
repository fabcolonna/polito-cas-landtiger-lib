#include "timer.h"
#include "LPC17xx.h"

#include <stdbool.h>
#include <stdlib.h>

// PRIVATE FUNCTIONS for MATCH REGISTERS

static void set_match_reg(LPC_TIM_TypeDef *const timer, const TIMER_MatchReg match_reg)
{
    switch (match_reg.which)
    {
    case 0:
        timer->MR0 = match_reg.match;
        break;
    case 1:
        timer->MR1 = match_reg.match;
        break;
    case 2:
        timer->MR2 = match_reg.match;
        break;
    case 3:
        timer->MR3 = match_reg.match;
        break;
    }

    timer->MCR |= match_reg.actions << (3 * match_reg.which);
}

static void unset_match_reg(LPC_TIM_TypeDef *const timer, const TIMER_MatchReg match_reg)
{
    switch (match_reg.which)
    {
    case 0:
        timer->MR0 = 0;
        break;
    case 1:
        timer->MR1 = 0;
        break;
    case 2:
        timer->MR2 = 0;
        break;
    case 3:
        timer->MR3 = 0;
        break;
    }

    timer->MCR &= ~(match_reg.actions << (3 * match_reg.which));
}

static void clear_match_regs(LPC_TIM_TypeDef *const timer)
{
    timer->MR0 = 0;
    timer->MR1 = 0;
    timer->MR2 = 0;
    timer->MR3 = 0;
    timer->MCR = 0;
}

// FUNCTIONS

void TIMER_Init(_OUT TIMER *timer, u8 which, u32 prescaler, u8 int_priority)
{
    if (!timer)
        return;

    bool valid_prio = !IS_DEF_PRIORITY(int_priority) && IS_BETWEEN_EQ(int_priority, 0, 15);
    bool valid_prescaler = !IS_NO_PRESCALER(prescaler);

    switch (which)
    {
    case 0:
        if (valid_prescaler)
            LPC_TIM0->PR = prescaler;

        NVIC_EnableIRQ(TIMER0_IRQn);

        if (valid_prio)
            NVIC_SetPriority(TIMER0_IRQn, int_priority);
        break;
    case 1:
        if (valid_prescaler)
            LPC_TIM1->PR = prescaler;

        NVIC_EnableIRQ(TIMER1_IRQn);

        if (valid_prio)
            NVIC_SetPriority(TIMER1_IRQn, int_priority);
        break;
    case 2:
        if (valid_prescaler)
            LPC_TIM2->PR = prescaler;

        NVIC_EnableIRQ(TIMER2_IRQn);

        if (valid_prio)
            NVIC_SetPriority(TIMER2_IRQn, int_priority);
        break;
    case 3:
        if (valid_prescaler)
            LPC_TIM3->PR = prescaler;

        NVIC_EnableIRQ(TIMER3_IRQn);

        if (valid_prio)
            NVIC_SetPriority(TIMER3_IRQn, int_priority);
        break;
    default:
        timer = NULL; // Invalid timer number
    }

    timer->which = which;
    timer->prescaler = prescaler;
}

void TIMER_Deinit(TIMER timer)
{
    TIMER_Disable(timer);
    TIMER_Reset(timer);

    switch (timer.which)
    {
    case 0:
        LPC_TIM0->PR = 0;
        clear_match_regs(LPC_TIM0);
        NVIC_DisableIRQ(TIMER0_IRQn);
        break;
    case 1:
        LPC_TIM1->PR = 0;
        clear_match_regs(LPC_TIM1);
        NVIC_DisableIRQ(TIMER1_IRQn);
        break;
    case 2:
        LPC_TIM2->PR = 0;
        clear_match_regs(LPC_TIM2);
        NVIC_DisableIRQ(TIMER2_IRQn);
        break;
    case 3:
        LPC_TIM3->PR = 0;
        clear_match_regs(LPC_TIM3);
        NVIC_DisableIRQ(TIMER3_IRQn);
        break;
    }
}

void TIMER_SetMatch(TIMER timer, TIMER_MatchReg match_reg)
{
    if (match_reg.which >= MR_COUNT || match_reg.which < 0)
        return;

    switch (timer.which)
    {
    case 0:
        set_match_reg(LPC_TIM0, match_reg);
        break;
    case 1:
        set_match_reg(LPC_TIM1, match_reg);
        break;
    case 2:
        set_match_reg(LPC_TIM2, match_reg);
        break;
    case 3:
        set_match_reg(LPC_TIM3, match_reg);
        break;
    }
}

void TIMER_UnsetMatch(TIMER timer, TIMER_MatchReg match_reg)
{
    switch (timer.which)
    {
    case 0:
        unset_match_reg(LPC_TIM0, match_reg);
        break;
    case 1:
        unset_match_reg(LPC_TIM1, match_reg);
        break;
    case 2:
        unset_match_reg(LPC_TIM2, match_reg);
        break;
    case 3:
        unset_match_reg(LPC_TIM3, match_reg);
        break;
    }
}

void TIMER_Enable(TIMER timer)
{
    // NB: Check if the reset bit is set in TCR. If so,
    // clear it otherwise the timer will never start.

    switch (timer.which)
    {
    case 0:
        CLR_BIT(LPC_TIM0->TCR, 1); // Clear reset bit
        SET_BIT(LPC_TIM0->TCR, 0);
        break;
    case 1:
        CLR_BIT(LPC_TIM1->TCR, 1);
        SET_BIT(LPC_TIM1->TCR, 0);
        break;
    case 2:
        CLR_BIT(LPC_TIM2->TCR, 1);
        SET_BIT(LPC_TIM2->TCR, 0);
        break;
    case 3:
        CLR_BIT(LPC_TIM3->TCR, 1);
        SET_BIT(LPC_TIM3->TCR, 0);
        break;
    }
}

void TIMER_Disable(TIMER timer)
{
    switch (timer.which)
    {
    case 0:
        CLR_BIT(LPC_TIM0->TCR, 0);
        break;
    case 1:
        CLR_BIT(LPC_TIM1->TCR, 0);
        break;
    case 2:
        CLR_BIT(LPC_TIM2->TCR, 0);
        break;
    case 3:
        CLR_BIT(LPC_TIM3->TCR, 0);
        break;
    }
}

void TIMER_Reset(TIMER timer)
{
    // Reset: bit 1 in TCR (bit 0 is left untouched)
    switch (timer.which)
    {
    case 0:
        SET_BIT(LPC_TIM0->TCR, 1);
        break;
    case 1:
        SET_BIT(LPC_TIM1->TCR, 1);
        break;
    case 2:
        SET_BIT(LPC_TIM2->TCR, 1);
        break;
    case 3:
        SET_BIT(LPC_TIM3->TCR, 1);
        break;
    }
}