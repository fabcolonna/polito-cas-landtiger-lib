#ifndef __RIT_TYPES_H
#define __RIT_TYPES_H

#define _RIT_JOB

/// @brief RIT interrupt function pointer for jobs that need to be
///        executed in the RIT interrupt handler.
typedef void (*RIT_InterruptJob)(void);

#endif