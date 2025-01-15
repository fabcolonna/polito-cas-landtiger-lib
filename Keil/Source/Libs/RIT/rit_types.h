#ifndef __RIT_TYPES_H
#define __RIT_TYPES_H

#define _RIT_JOB

/// @brief Tells the RIT that the specified job needs to be executed
///        at exactly the interval of interrupts chosen during initialization.
#define RIT_NO_DIVIDER 1

typedef enum
{
    /// @brief No errors.
    RIT_ERR_OK = 0,
    /// @brief An attempt was made to assign another job to the RIT,
    ///        but the data structure is full.
    RIT_ERR_NO_MORE_SPACE_FOR_JOBS,

    /// @brief An invalid priority was specified (must be 0 <= prio <= 15).
    ///        This is not necessarily an error, could be treated as an alert that
    ///        the custom priority has been ignored in favor of the default value.
    RIT_ERR_INT_PRIO_INVALID,

    /// @brief The job specified was not found in the handler queue.
    RIT_ERR_JOB_NOT_FOUND,
} RIT_Error;

/// @brief RIT interrupt function pointer for jobs that need to be
///        executed in the RIT interrupt handler.
typedef void (*RIT_Job)(void);

/// @brief An ID associated to a registered job.
// typedef i16 RIT_JobID;

#endif