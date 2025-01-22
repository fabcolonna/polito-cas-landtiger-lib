#ifndef __RIT_H
#define __RIT_H

#include "allocator.h"
#include "rit_types.h"
#include "types.h"

#include <stdbool.h>

/// @brief Initialize the RIT
/// @param alloc The memory allocator to use for the RIT job list.
/// @param ival_ms Interval in millis between each RIT interrupt.
/// @param int_priority Debouncer interrupt priority (0 (highest), 15 (lowest)). If set to
///        INT_PRIO_DEF, the default priority will be used.
/// @note The RIT has a fixed number of jobs that can be added to the job queue, defined by
///       RIT_JOBS_COUNT (20 by default) in the peripherals.h file.
/// @return RIT_Error The error.
RIT_Error RIT_Init(MEM_Allocator *const alloc, u32 ival_ms, u16 int_priority);

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
/// @param ival_multiplier The factor to multiply the base interval by to determine the job's interval
/// @example Base interval = 10ms, ival_multiplier = 2, job interval = 20ms.
/// @return RIT_Error The error.
/// @note The multiplier can never be used to set the job interval to less than the base interval.
///       If you need a job that runs faster, re-initialize the RIT with a smaller base interval, and
///       use the multiplier to slow down the other jobs.
RIT_Error RIT_AddJob(RIT_Job job, u8 multiplier_factor);

/// @brief Removes a job from the RIT interrupt handler job queue.
/// @param job The job to remove
RIT_Error RIT_RemoveJob(RIT_Job job);

/// @brief Returns the number of jobs in the job queue.
u32 RIT_GetJobsCount(void);

/// @brief Returns the multiplier factor for the given job.
/// @param job The job to get the multiplier factor from
/// @return u8 The multiplier factor
u8 RIT_GetJobMultiplierFactor(RIT_Job job);

/// @brief Sets the multiplier factor for the given job.
/// @param job The job to set the multiplier factor for
/// @param multiplier_factor The new multiplier factor
/// @return RIT_Error The error.
RIT_Error RIT_SetJobMultiplierFactor(RIT_Job job, u8 multiplier_factor);

/// @brief Clears the job queue.
void RIT_ClearJobs(void);

#endif