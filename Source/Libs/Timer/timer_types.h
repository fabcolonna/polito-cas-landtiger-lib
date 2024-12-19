#ifndef __TIMER_TYPES_H
#define __TIMER_TYPES_H

#include "utils.h"
#include <stdint.h>

#define TIMERS_COUNT 4
#define MR_COUNT 4

typedef enum
{
    TIMER_0 = 0,
    TIMER_1,
    TIMER_2,
    TIMER_3
} TIMER_Which;

typedef enum
{
    TIM_MR_NOP = 0,
    TIM_MR_INT = 1 << 0,
    TIM_MR_RES = 1 << 1,
    TIM_MR_STP = 1 << 2
} TIMER_MR_Action;

typedef struct
{
    u8 which;
    u32 match;
    u8 actions;
} TIMER_MatchReg;

typedef struct
{
    u8 which;
    u32 prescaler;
    // Future: implement Capture registers & CCR
    // Future: implement EMR
} TIMER;

// INTERRUPTS

// Refer to Table 426 in the manual for more info.
typedef enum
{
    TIM_INT_SRC_MR0 = 0, // Bit 0, MR0
    TIM_INT_SRC_MR1,     // Bit 1, MR1
    TIM_INT_SRC_MR2,
    TIM_INT_SRC_MR3,

    // Unsupported, Capture channels
    // TIM_INT_SRC_CR0 = 0x10,
    // TIM_INT_SRC_CR1 = 0x20
    TIM_INT_SRC_COUNT
} TIMER_Interrupt_Source;

// Interrupt handler function pointer
typedef void (*TIMER_InterruptHandler)(void);

#endif