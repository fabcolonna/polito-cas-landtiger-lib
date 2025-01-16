#include "peripherals.h" // Contains the RIT_JOBS_COUNT define.
#include "rit.h"

#include <LPC17xx.h>
#include <stdlib.h>
#include <string.h>

// PRIVATE TYPES

typedef struct
{
    RIT_Job job;

    // Interval in milliseconds between each execution of the job. It is used to
    // determine if the job should be executed in the interrupt handler, based on
    // the RIT counter value.
    u32 interval;
    bool enabled;
} JobWrapper;

/// @brief External variable that stores the base interval between each RIT interrupt.
/// @note This variable is used to calculate the RIT counter value for each job.
_DECL_EXTERNALLY u32 base_ival;

/// @brief Array of jobs to execute in the RIT interrupt handler.
_PRIVATE JobWrapper jobs[RIT_JOBS_COUNT] = {0};
_PRIVATE u8 jobs_sz = 0;

/// @brief Stores the current RIT counter value, and it's incremented by each RIT interrupt.
///        It's used with the counter_reset_value, that's the maximum value the counter can
///        reach before being reset, which corresponds to the interval of the less frequent job.
_PRIVATE u32 counter = 0;
_USED_EXTERNALLY u32 counter_reset_value;

// PRIVATE FUNCTIONS

_PRIVATE void recalculate_reset_value(void)
{
    counter_reset_value = base_ival;
    for (u16 i = 0; i < jobs_sz; i++)
    {
        if (!jobs[i].enabled || !jobs[i].job)
            continue;

        if (jobs[i].interval > counter_reset_value)
            counter_reset_value = jobs[i].interval;
    }
}

// PUBLIC FUNCTIONS

RIT_Error RIT_EnableJob(RIT_Job job)
{
    for (u16 i = 0; i < jobs_sz; i++)
    {
        if (jobs[i].job == job)
        {
            jobs[i].enabled = true;

            // Need to recompute the counter_reset_value, if the interval
            // of the enabled job is higher than the current least frequent enabled job.
            if (jobs[i].interval > counter_reset_value)
                counter_reset_value = jobs[i].interval;

            return RIT_ERR_OK;
        }
    }

    // If here, job was not found
    return RIT_ERR_JOB_NOT_FOUND;
}

RIT_Error RIT_DisableJob(RIT_Job job)
{
    for (u16 i = 0; i < jobs_sz; i++)
    {
        if (jobs[i].job == job)
        {
            jobs[i].enabled = false;

            // Need to recompute the counter_reset_value, if this job was
            // the slowest one.
            if (jobs[i].interval == counter_reset_value)
                recalculate_reset_value();

            return RIT_ERR_OK;
        }
    }

    return RIT_ERR_JOB_NOT_FOUND;
}

RIT_Error RIT_AddJob(RIT_Job job, u8 divider_factor)
{
    if (jobs_sz == RIT_JOBS_COUNT)
        return RIT_ERR_NO_MORE_SPACE_FOR_JOBS;

    const JobWrapper new = {
        .job = job,
        .interval = base_ival * divider_factor, // divider_factor less frequent trigger.
        .enabled = false,
    };

    jobs[jobs_sz++] = new;

    // Compute the maximum value counter can reach before being reset
    if (new.interval > counter_reset_value)
        counter_reset_value = new.interval;

    return RIT_ERR_OK;
}

RIT_Error RIT_RemoveJob(RIT_Job job)
{
    for (u8 i = 0; i < jobs_sz; i++)
    {
        if (jobs[i].job == job)
        {
            if (i < jobs_sz - 1)
                memmove(jobs + i, jobs + i + 1, (jobs_sz - i - 1) * sizeof(RIT_Job));

            jobs_sz--;
            break;
        }
    }

    // Recompute the maximum value counter can reach before being reset
    recalculate_reset_value();
    return RIT_ERR_OK;
}

void RIT_ClearJobs(void)
{
    jobs_sz = 0;
    counter_reset_value = base_ival;
}

// INTERRUPT HANDLER

extern void RIT_IRQHandler(void)
{
#ifdef SIMULATOR
    RIT_Disable();
#endif

    LPC_RIT->RICOUNTER = 0;      // Reset counter
    SET_BIT(LPC_RIT->RICTRL, 0); // Clear interrupt flag

    if (jobs_sz <= 0)
        return;

    if (jobs_sz > 0)
    {
        counter += base_ival;
        for (u16 i = 0; i < jobs_sz; i++)
        {
            if (!jobs[i].enabled)
                continue;

            if (counter % jobs[i].interval == 0 && jobs[i].job)
                jobs[i].job();
        }

        // Reset the counter if the maximum value is reached
        if (counter >= counter_reset_value)
            counter = 0;
				
    }

#ifdef SIMULATOR
    RIT_Enable();
#endif
}
