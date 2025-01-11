#ifndef __RIT_H
#define __RIT_H

#include "rit_types.h"
#include "utils.h"

#include <stdbool.h>

/// @brief Initialize the RIT
/// @param ival_ms Interval in millis between each RIT interrupt
/// @param int_priority Debouncer interrupt priority (0 (highest), 15 (lowest)). If set to
///        INT_PRIO_DEF, the default priority will be used.
/// @note The RIT has a fixed number of jobs that can be added to the job queue, defined by
///       RIT_JOBS_COUNT (20 by default).
void RIT_Init(u32 ival_ms, u16 int_priority);
void RIT_Deinit(void);

void RIT_Enable(void);
void RIT_Disable(void);
bool RIT_IsEnabled(void);



/// @brief Adds a job to the RIT interrupt handler job queue, if there is space.
/// @param job The job to execute
/// @param divider_factor The factor to multiply the base interval by to determine the job's interval
///                       Example: base interval = 10ms, divider_factor = 2, job interval = 20ms.
/// @return true if the job was added, false otherwise
/// @note The divider factor can never be used to set the job interval to less than the base interval.
///       If you need a job that runs faster, re-initialize the RIT with a smaller base interval, and
///       use the divider factor to slow down the other jobs.
bool RIT_AddJob(RIT_InterruptJob job, u32 divider_factor);

/// @brief Removes a job from the RIT interrupt handler job queue.
/// @param job The job to remove
void RIT_RemoveJob(RIT_InterruptJob job);

/// @brief Clears the job queue.
void RIT_ClearJobs(void);

#endif