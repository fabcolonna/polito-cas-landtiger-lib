#include "cl_list.h"
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
_PRIVATE CL_List *jobs;

/// @brief Stores the current RIT counter value, and it's incremented by each RIT interrupt.
///        It's used with the counter_reset_value, that's the maximum value the counter can
///        reach before being reset, which corresponds to the interval of the less frequent job.
_PRIVATE u32 counter = 0;
_USED_EXTERNALLY u32 counter_reset_value;

// PRIVATE FUNCTIONS

// Used externally (in rit.c)
void allocate_jobs_list(MEM_Allocator *const alloc)
{
    jobs = CL_ListAlloc(alloc, sizeof(JobWrapper));
    if (!jobs)
        return;
}

// Used externally (in rit.c)
void free_jobs_list(void)
{
    if (jobs)
        CL_ListFree(jobs);
}

_PRIVATE void recalculate_reset_value(void)
{
    counter_reset_value = base_ival;
    CL_LIST_FOREACH_PTR(JobWrapper, job, jobs, {
        if (!job->enabled || !job->job)
            continue;

        if (job->interval > counter_reset_value)
            counter_reset_value = job->interval;
    });
}

// PUBLIC FUNCTIONS

RIT_Error RIT_EnableJob(RIT_Job job)
{
    CL_LIST_FOREACH_PTR(JobWrapper, list_job, jobs, {
        if (list_job->job == job)
        {
            list_job->enabled = true;

            // Need to recompute the counter_reset_value, if the interval
            // of the enabled job is higher than the current least frequent enabled job.
            if (list_job->interval > counter_reset_value)
                counter_reset_value = list_job->interval;

            return RIT_ERR_OK;
        }
    });

    // If here, job was not found
    return RIT_ERR_JOB_NOT_FOUND;
}

RIT_Error RIT_DisableJob(RIT_Job job)
{
    CL_LIST_FOREACH_PTR(JobWrapper, list_job, jobs, {
        if (list_job->job == job)
        {
            list_job->enabled = false;

            // Need to recompute the counter_reset_value, if this job was
            // the slowest one.
            if (list_job->interval == counter_reset_value)
                recalculate_reset_value();

            return RIT_ERR_OK;
        }
    });

    return RIT_ERR_JOB_NOT_FOUND;
}

RIT_Error RIT_AddJob(RIT_Job job, u8 divider_factor)
{
    const u32 ival = base_ival * divider_factor;
    if (CL_ListPushBack(jobs, &(JobWrapper){.job = job, .interval = ival, .enabled = false}) != CL_ERR_OK)
        return RIT_ERR_DURING_PUSHBACK;

    // Compute the maximum value counter can reach before being reset
    if (ival > counter_reset_value)
        counter_reset_value = ival;

    return RIT_ERR_OK;
}

RIT_Error RIT_RemoveJob(RIT_Job job)
{
    for (u32 i = 0; i < CL_ListSize(jobs); i++)
    {
        JobWrapper *const list_job;
        CL_ListGetPtr(jobs, i, (void **)&(list_job));
        {
            if (list_job->job == job)
            {
                if (CL_ListRemoveAt(jobs, i, ((void *)0)) != CL_ERR_OK)
                    return RIT_ERR_DURING_REMOVEAT;

                recalculate_reset_value();
                break;
            }
        }
    }

    return RIT_ERR_OK;
}

u32 RIT_GetJobsCount(void)
{
    return CL_ListSize(jobs);
}

u8 RIT_GetJobMultiplierFactor(RIT_Job job)
{
    CL_LIST_FOREACH_PTR(JobWrapper, list_job, jobs, {
        if (list_job->job == job)
            return list_job->interval / base_ival;
    })

    return 0;
}

RIT_Error RIT_SetJobMultiplierFactor(RIT_Job job, u8 multiplier_factor)
{
    if (multiplier_factor == 0)
        return RIT_ERR_INVALID_MULTIPLIER;

    CL_LIST_FOREACH_PTR(JobWrapper, list_job, jobs, {
        if (list_job->job == job)
        {
            list_job->interval = base_ival * multiplier_factor;

            // Need to recompute the counter_reset_value, if this job was
            // the slowest one.
            if (list_job->interval == counter_reset_value)
                recalculate_reset_value();

            return RIT_ERR_OK;
        }
    });

    return RIT_ERR_JOB_NOT_FOUND;
}

void RIT_ClearJobs(void)
{
    CL_ListClear(jobs);
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

    if (CL_ListSize(jobs) <= 0)
        return;

    counter += base_ival;
    CL_LIST_FOREACH_PTR(JobWrapper, job, jobs, {
        if (!job->enabled)
            continue;

        if (counter % job->interval == 0 && job->job)
            job->job();
    });

    // Reset the counter if the maximum value is reached
    if (counter >= counter_reset_value)
        counter = 0;

#ifdef SIMULATOR
    RIT_Enable();
#endif
}
