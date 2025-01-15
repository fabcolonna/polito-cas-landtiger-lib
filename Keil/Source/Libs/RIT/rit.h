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
///       RIT_JOBS_COUNT (20 by default) in the peripherals.h file.
/// @return RIT_Error The error.
RIT_Error RIT_Init(u32 ival_ms, u16 int_priority);

/// @brief De-initializes the RIT, by removing every job and disconnecting power.
void RIT_Deinit(void);

/// @brief Enables the RIT counting.
void RIT_Enable(void);

/// @brief Disables the RIT entirely (stops counting).
void RIT_Disable(void);

/// @brief Returns whether the RIT is counting or not.
bool RIT_IsEnabled(void);

/// @brief Returns the interval in millis that RIT uses for its interrupts
u32 RIT_GetIntervalMs(void);

// JOBS HANDLING

/// @brief Include this job in the RIT handler queue.
RIT_Error RIT_EnableJob(RIT_Job job);

/// @brief Exclude this job from the RIT handler queue.
RIT_Error RIT_DisableJob(RIT_Job job);

/// @brief Adds a job to the RIT interrupt handler job queue, if there is space.
/// @param job The job to execute
/// @param divider_factor The factor to multiply the base interval by to determine the job's interval
/// @example Base interval = 10ms, divider_factor = 2, job interval = 20ms.
/// @return RIT_Error The error.
/// @note The divider factor can never be used to set the job interval to less than the base interval.
///       If you need a job that runs faster, re-initialize the RIT with a smaller base interval, and
///       use the divider factor to slow down the other jobs.
RIT_Error RIT_AddJob(RIT_Job job, u8 divider_factor);

/// @brief Removes a job from the RIT interrupt handler job queue.
/// @param job The job to remove
RIT_Error RIT_RemoveJob(RIT_Job job);

/// @brief Clears the job queue.
void RIT_ClearJobs(void);

#endif