#ifndef __CL_TYPES_H
#define __CL_TYPES_H

// COMPARE FUNCTION

/// @brief A function pointer type for comparing two elements.
typedef int (*CL_CompareFn)(const void *, const void *);

#define _COMPARE_FN(__name, __a_param, __b_param, ...)                                                                 \
    _PRIVATE int __name(const void *const __a_param, const void *const __b_param) __VA_ARGS__

// ERRORS

/// @brief Specify this in the size parameter of the collections initialization to
///        tell the library that the number of elements is unknown, and thus it
///        should automatically resize the collection as needed.
/// @note Always try to specify the size if possible, as this will potentially save
///       memory and improve performance.
// #define CL_AUTO_SIZE 0
// #define CL_AUTO_SIZE_BASE_COUNT 10

/// @brief Error codes for the Collections library.
typedef enum
{
    /// @brief No error.
    CL_ERR_OK = 0,

    /// @brief Invalid arguments.
    CL_ERR_INVALID_PARAMS,

    /// @brief Out of bounds.
    CL_ERR_OUT_OF_BOUNDS,

    /// @brief No memory available.
    CL_ERR_NO_MEMORY,

    /// @brief Not found.
    CL_ERR_NOT_FOUND,

    /// @brief Empty collection.
    CL_ERR_EMPTY,
} CL_Error;

#endif