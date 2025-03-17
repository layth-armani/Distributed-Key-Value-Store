/**
 * @file error.h
 * @brief error codes
 *
 * @author J.-C. Chappelier, E. Bugnion
 * @date 2017-2024
 */

#pragma once

#include <assert.h>
#include <stdio.h> // for fprintf

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief internal error codes.
 *
 */
#define ERR_NONE 0

enum error_codes {
    ERR_FIRST = -128, // not an actual error but to set the first error number
    ERR_IO,
    ERR_RUNTIME,
    ERR_OUT_OF_MEMORY,
    ERR_NETWORK,
    ERR_INVALID_CONFIG,
    ERR_INVALID_COMMAND,
    ERR_INVALID_ARGUMENT, /* internal error, when a function receives incorrect
                           * parameters (e.g. null pointer)                      */
    ERR_ADDRESS,
    ERR_NOT_FOUND,
    ERR_THREADING,
    ERR_LAST // not an actual error but to have e.g. the total number of errors
};

/*
 * Helpers (macros)
 */

// example: ASSERT(x>0);

#define ASSERT assert

#ifdef DEBUG
#define debug_printf(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define debug_printf(fmt, ...)                                                 \
    do {                                                                       \
    } while (0)
#endif

#define M_REQUIRE_NON_NULL(arg)                                                \
    do {                                                                       \
        if (arg == NULL) {                                                     \
            debug_printf("ERROR: parameter %s is NULL when calling  %s() "     \
                         "(defined in %s)\n",                                  \
                         #arg, __func__, __FILE__);                            \
            return ERR_INVALID_ARGUMENT;                                       \
        }                                                                      \
    } while (0)

#define dkvs_printf printf

/**
 * @brief filesystem internal error messages. Defined in error.c.
 *        Should be accessed using `ERR_MSG` (see below)
 *
 */
extern const char *const ERR_MESSAGES[];

/**
 * @brief yields the error message corresponding to an error code
 */
#define ERR_MSG(err)                                                           \
    (ERR_FIRST < (err) && (err) < ERR_LAST ? ERR_MESSAGES[(err) - ERR_FIRST]   \
                                           : "Invalid error code")

#ifdef __cplusplus
}
#endif
