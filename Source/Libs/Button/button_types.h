#ifndef __BUTTON_TYPES_H
#define __BUTTON_TYPES_H

// INTERRUPTS

typedef enum
{
    /// @brief INT0: Third button from the left (next to RES)
    BTN_INT_SRC_EINT0 = 0,

    /// @brief EINT1: First button from the left
    BTN_INT_SRC_EINT1,

    /// @brief EINT2: Second button from the left
    BTN_INT_SRC_EINT2,

    /// @brief Number of interrupt sources
    BTN_INT_SRC_COUNT
} BUTTON_Interrupt_Source;

// Interrupt handler function pointer
typedef void (*BUTTON_Interrupt_Handler)(void);

#endif