#include "rit.h"
#include <LPC17xx.h>

#include <stdlib.h>
#include <string.h>

// PRIVATE TYPES

typedef struct
{
    RIT_InterruptJob job;

    // Interval in milliseconds between each execution of the job. It is used to
    // determine if the job should be executed in the interrupt handler, based on
    // the RIT counter value.
    u32 interval;
} RIT_JobWrapper;

#define RIT_JOBS_COUNT 20

/// @brief External variable that stores the base interval between each RIT interrupt.
/// @note This variable is used to calculate the RIT counter value for each job.
_DECL_EXTERNALLY u32 base_ival;

/// @brief Array of jobs to execute in the RIT interrupt handler.
_PRIVATE RIT_JobWrapper jobs[RIT_JOBS_COUNT] = {NULL};
_PRIVATE u8 jobs_sz = 0;

/// @brief Stores the current RIT counter value, and it's incremented by each RIT interrupt.
///        It's used with the counter_reset_value, that's the maximum value the counter can
///        reach before being reset, which corresponds to the interval of the less frequent job.
_PRIVATE u32 counter = 0, counter_reset_value = 0;

// PUBLIC FUNCTIONS

bool RIT_AddJob(RIT_InterruptJob job, u32 divider_factor)
{
    if (jobs_sz == RIT_JOBS_COUNT)
        return false;

    RIT_JobWrapper job_wrapper = {job, base_ival * divider_factor};
    jobs[jobs_sz++] = job_wrapper;

    // Compute the maximum value counter can reach before being reset
    if (job_wrapper.interval > counter_reset_value)
        counter_reset_value = job_wrapper.interval;

    return true;
}

void RIT_RemoveJob(RIT_InterruptJob job)
{
    for (u8 i = 0; i < jobs_sz; i++)
    {
        if (jobs[i].job == job)
        {
            if (i < jobs_sz - 1)
                memmove(jobs + i, jobs + i + 1, (jobs_sz - i - 1) * sizeof(RIT_InterruptJob));

            jobs_sz--;
            break;
        }
    }

    // Recompute the maximum value counter can reach before being reset
    counter_reset_value = 0;
    for (u8 i = 0; i < jobs_sz; i++)
    {
        if (jobs[i].interval > counter_reset_value)
            counter_reset_value = jobs[i].interval;
    }
}

void RIT_ClearJobs(void)
{
    jobs_sz = 0;
}

// INTERRUPT HANDLER

extern void RIT_IRQHandler(void)
{
    if (jobs_sz > 0)
    {
        counter += base_ival;
        RIT_InterruptJob job;
        for (u8 i = 0; i < jobs_sz; i++)
        {
            job = jobs[i].job;
            if (counter % jobs[i].interval == 0 && job)
                job();
        }

        // Reset the counter if the maximum value is reached
        if (counter >= counter_reset_value)
            counter = 0;
    }

    LPC_RIT->RICOUNTER = 0;      // Reset counter
    SET_BIT(LPC_RIT->RICTRL, 0); // Clear interrupt flag
}
