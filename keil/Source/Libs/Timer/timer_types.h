#ifndef __TIMER_TYPES_H
#define __TIMER_TYPES_H

#include "utils.h"
#include <stdint.h>

#define MR_COUNT 4

typedef enum
{
    TIM_0 = 0,
    TIM_1,
    TIM_2,
    TIM_3,
    TIM_COUNT
} TIMER_Which;

typedef enum
{
    TIM_MR0 = 0,
    TIM_MR1,
    TIM_MR2,
    TIM_MR3,
    TIM_MR_COUNT
} TIMER_MRWhich;

typedef enum
{
    TIM_MR_NOP = 0,
    TIM_MR_INT = 1 << 0,
    TIM_MR_RES = 1 << 1,
    TIM_MR_STP = 1 << 2
} TIMER_MRAction;

typedef struct
{
    u8 which;
    u32 match;
    u8 actions;
} TIMER_MatchRegister;

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
} TIMER_InterruptSource;

// Interrupt handler function pointer
typedef void (*TIMER_InterruptHandler)(void);

#endif