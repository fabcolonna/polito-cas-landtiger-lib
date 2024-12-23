#ifndef __RIT_H
#define __RIT_H

#include "rit_types.h"
#include "utils.h"

#include <stdbool.h>

/// @brief Initialize the RIT
/// @param ival_ms Interval in millis between each RIT interrupt
/// @param int_priority Debouncer interrupt priority (0 (highest), 15 (lowest)). If set to
///        INT_PRIO_DEF, the default priority will be used.
void RIT_Init(u32 ival_ms, u16 int_priority);
void RIT_Deinit(void);

void RIT_Enable(void);
void RIT_Disable(void);
bool RIT_IsEnabled(void);

/// @brief Adds a job to the RIT interrupt handler job queue, if there is space.
/// @param job The job to execute
/// @return true if the job was added, false otherwise
bool RIT_AddJob(RIT_InterruptJob job);

/// @brief Removes a job from the RIT interrupt handler job queue.
/// @param job The job to remove
void RIT_RemoveJob(RIT_InterruptJob job);

/// @brief Clears the job queue.
void RIT_ClearJobs(void);

#endif