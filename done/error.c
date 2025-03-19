/**
 * @file error.c
 * @brief error messages
 * @author J.-C. Chappelier, E. Bugnion
 * @date 2017-2024
 */

const char *const ERR_MESSAGES[] = {
    "", // no error
    "I/O Error",
    "Runtime error",
    "(re|m|c)alloc failed",
    "Not enough arguments",
    "Network error",
    "Invalid config",
    "Invalid command",
    "Invalid argument (internal error)",
    "Invalid address",
    "Not found",
    "Threading error",
    "no error (shall not be displayed)" // ERR_LAST
};
