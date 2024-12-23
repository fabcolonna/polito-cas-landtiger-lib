#include "rit.h"
#include <LPC17xx.h>

#include <stdlib.h>
#include <string.h>

#define RIT_JOBS_COUNT 10

/// @brief Array of jobs to execute in the RIT interrupt handler.
_PRIVATE RIT_InterruptJob jobs[RIT_JOBS_COUNT] = {NULL};
_PRIVATE u8 jobs_sz = 0;

// PUBLIC FUNCTIONS

bool RIT_AddJob(RIT_InterruptJob job)
{
    if (jobs_sz == RIT_JOBS_COUNT)
        return false;

    jobs[jobs_sz++] = job;
    return true;
}

void RIT_RemoveJob(RIT_InterruptJob job)
{
    for (u8 i = 0; i < jobs_sz; i++)
    {
        if (jobs[i] == job)
        {
            if (i < jobs_sz - 1)
                memmove(jobs + i, jobs + i + 1, (jobs_sz - i - 1) * sizeof(RIT_InterruptJob));

            jobs_sz--;
            break;
        }
    }
}

void RIT_ClearJobs(void)
{
    jobs_sz = 0;
}

// INTERRUPT HANDLER

extern void RIT_IRQHandler(void)
{
    RIT_InterruptJob job;
    for (u8 i = 0; i < jobs_sz; i++)
    {
        job = jobs[i];
        if (job)
            job();
    }

    LPC_RIT->RICOUNTER = 0;      // Reset counter
    SET_BIT(LPC_RIT->RICTRL, 0); // Clear interrupt flag
}
