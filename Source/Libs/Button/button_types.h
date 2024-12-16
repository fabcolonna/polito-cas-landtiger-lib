#ifndef __BUTTON_TYPES_H
#define __BUTTON_TYPES_H

// INTERRUPTS

typedef enum
{
    BTN_INT_SRC_EINT0 = 0,
    BTN_INT_SRC_EINT1,
    BTN_INT_SRC_EINT2,
    BTN_INT_SRC_COUNT
} BUTTON_Interrupt_Source;

// Interrupt handler function pointer
typedef void (*BUTTON_Interrupt_Handler)(void);

#endif