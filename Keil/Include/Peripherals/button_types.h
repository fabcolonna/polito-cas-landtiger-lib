#ifndef __BUTTONS_TYPES_H
#define __BUTTONS_TYPES_H

typedef enum
{
    BTN_DEBOUNCE_WITH_RIT = 0x1
} BUTTON_Config;

typedef enum
{
    /// @brief No error.
    BTN_ERR_OK = 0,
    /// @brief During initialization, DEBOUNCE_WITH_RIT was requested
    ///        but the RIT was not enabled.
    BTN_ERR_RIT_UNINIT,

    /// @brief An invalid priority was specified (must be 0 <= prio <= 15).
    ///        This is not necessarily an error, could be treated as an alert that
    ///        the custom priority has been ignored in favor of the default value.
    BTN_ERR_INT_PRIO_INVALID,

    /// @brief It was requested to enable a button interrupt, but no function
    ///        was bound to it.
    BTN_ERR_NO_HANDLER_TO_ENABLE,
} BUTTON_Error;

// INTERRUPTS

typedef enum
{
    /// @brief INT0: Third button from the left (next to RES)
    BTN_SRC_EINT0 = 0,

    /// @brief EINT1: First button from the left
    BTN_SRC_EINT1,

    /// @brief EINT2: Second button from the left
    BTN_SRC_EINT2,

    BTN_SRC_COUNT
} BUTTON_Source;

// Interrupt handler function pointer
typedef void (*BUTTON_Function)(void);

#endif