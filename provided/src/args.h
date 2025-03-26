#pragma once

/**
 * @file args.h
 * @brief Parsing argv options.
 *
 * @author Valérian Rousset
 */

#include <stddef.h> // for size_t

/**
 * @brief contains parsed arguments
 */
struct args {
    size_t total_servers; /* N: number of servers storing a key-value pair
                           *    = maximum number of servers to contact     */
    size_t get_needed;    // R: (read) minumum number of servers to get() from
    size_t put_needed;    // W: (put)  minumum number of servers to put() to
};
typedef struct args args_t;

/**
 * @brief default values for N, R and W.
 */
#define DKVS_DEFAULT_N 3
#define DKVS_DEFAULT_R 2
#define DKVS_DEFAULT_W 2

/**
 * @brief enum for the supported_args flag of parse_opt_args
 */
typedef enum {
    TOTAL_SERVERS = 1 << 0, // N is supported
    GET_NEEDED    = 1 << 1, // R is supported
    PUT_NEEDED    = 1 << 2  // W is supported
} args_kind;

/**
 * @brief parse optional arguments
 * @param args (output) the args_t struct to be initialized
 * @param supported_args OR'ed args_kind flags of supported options
 * @param argc (output) the number of arguments remaining (after optional arguments have been parsed)
 * @param rem_argv (output) the array of remaining arguments
 * @return an error code
 */
int parse_opt_args(args_t *args, size_t supported_args, int *argc, char ***rem_argv);
