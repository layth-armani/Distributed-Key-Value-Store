#pragma once

/**
 * @file tests.h
 * @brief CS-202 utilities for tests
 *
 * @author Valérian Rousset, J.-C. Chappelier, E. Bugnion, L. Mermod
 * @date 2017-2024
 */

#include "error.h"

#include <arpa/inet.h>
#include <check.h>
#include <stdio.h>
#include <stdlib.h> // EXIT_FAILURE

#if CHECK_MINOR_VERSION >= 13
#define TEST_FUNCTION_POSTFIX "_fn"
#else
#define TEST_FUNCTION_POSTFIX ""
#endif

static const char *const ERR_NAMES[] = {
    "ERR_FIRST",
    "ERR_IO",
    "ERR_RUNTIME",
    "ERR_OUT_OF_MEMORY",
    "ERR_NETWORK",
    "ERR_INVALID_CONFIG",
    "ERR_INVALID_COMMAND",
    "ERR_INVALID_ARGUMENT",
    "ERR_ADDRESS",
    "ERR_NOT_FOUND",
    "ERR_THREADING",
    "ERR_LAST"};

#define ERR_NAME(err)                                                                            \
    (err < ERR_FIRST || ERR_LAST < err ? err > 0 ? "VALUE" : (err == 0 ? "ERR_NONE" : "UNKNOWN") \
                                       : ERR_NAMES[err - ERR_FIRST])

#define ck_assert_err_core(value, op, err)                                                                         \
    do {                                                                                                           \
        int __value = (value);                                                                                     \
        if (!(__value op err)) {                                                                                   \
            ck_abort_msg("Assertion %s " #op " %s failed, got %s (%d)", #value, #err, ERR_NAME(__value), __value); \
        }                                                                                                          \
        mark_point();                                                                                              \
    } while (0)

#define ck_assert_err(value, err) ck_assert_err_core(value, ==, err)

#define ck_assert_fails(value) ck_assert_err_core(value, <, ERR_NONE)

#define ck_assert_invalid_arg(value) ck_assert_err(value, ERR_INVALID_ARGUMENT)

#define ck_assert_err_mem(value) ck_assert_err(value, ERR_NOMEM)

#define ck_assert_err_none(value) ck_assert_err(value, ERR_NONE)

#ifndef ck_assert_ptr_nonnull
#define ck_assert_ptr_nonnull(ptr) ck_assert_ptr_ne(ptr, NULL)
#endif

#ifndef ck_assert_ptr_null
#define ck_assert_ptr_null(ptr) ck_assert_ptr_eq(ptr, NULL)
#endif

#define Add_Case(S, C, Title)       \
    TCase *C = tcase_create(Title); \
    suite_add_tcase(S, C)

#define Add_Test(S, Title)         \
    do {                           \
        Add_Case(S, tc, #Title);   \
        tcase_add_test(tc, Title); \
    } while (0)

#define Add_Test_With_Fixture(S, Title, setup, teardown) \
    do {                                                 \
        Add_Case(S, tc, #Title);                         \
        tcase_add_test(tc, Title);                       \
        tcase_add_checked_fixture(tc, setup, teardown);  \
    } while (0)

#define TEST_SUITE(get_suite)                                                                      \
    int main(void)                                                                                 \
    {                                                                                              \
        SRunner *sr = srunner_create(get_suite());                                                 \
        srunner_run_all(sr, CK_VERBOSE);                                                           \
                                                                                                   \
        int number_failed = srunner_ntests_failed(sr);                                             \
                                                                                                   \
        if (number_failed > 0) {                                                                   \
            TestResult **results = srunner_failures(sr);                                           \
                                                                                                   \
            puts("\033[31m==================== SUMMARY ====================\033[000m");            \
                                                                                                   \
            for (int i = 0; i < number_failed; ++i) {                                              \
                char buf[4097] = {0}; /* Max path length on Unix is 4096 */                        \
                strcat(buf, __FILE__);                                                             \
                buf[strlen(buf) - 2] = 0; /* skip the trailing '.c' */                             \
                                                                                                   \
                printf("\033[31m|\033[000m Test %s failed. To run in gdb, use: \033[001mmake dbg " \
                       "TEST=%s" TEST_FUNCTION_POSTFIX " "                                         \
                       "EXE=%s\033[000m\n",                                                        \
                       tr_tcname(results[i]), tr_tcname(results[i]), buf + 10);                    \
            }                                                                                      \
                                                                                                   \
            puts("\033[31m=================================================\033[000m");            \
                                                                                                   \
            free(results);                                                                         \
        }                                                                                          \
                                                                                                   \
        srunner_free(sr);                                                                          \
        return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;                                 \
    }

#ifdef WITH_PRINT
#define test_print(...) printf(__VA_ARGS__)
#else
#define test_print(...) \
    do {                \
    } while (0)
#endif

#define start_test_print test_print("=== %s:\n", __func__)
#define end_test_print test_print("=== END of %s:\n", __func__)

#define NON_NULL ((void *)1)

static void read_file(void *buffer, const char *filename, size_t size)
{
    FILE *file = fopen(filename, "r");

    ck_assert_ptr_nonnull(file);
    ck_assert_uint_eq(fread(buffer, 1, size, file), size);

    fclose(file);
}

static void read_file_and_size(void **buffer, const char *filename, size_t *size)
{
    FILE *file = fopen(filename, "r");
    ck_assert_ptr_nonnull(file);

    ck_assert_int_eq(fseek(file, 0, SEEK_END), 0);
    *size = (size_t)ftell(file);
    rewind(file);

    *buffer = calloc(*size, 1);
    ck_assert_ptr_nonnull(buffer);

    ck_assert_uint_eq(fread(*buffer, 1, *size, file), *size);

    fclose(file);
}

#define ck_assert_node_eq(node, ip, port, _sha)              \
    do {                                                     \
        ck_assert_mem_eq(node.sha, _sha, SHA_DIGEST_LENGTH); \
    } while (0)
    
#define ck_assert_two_nodes_eq(node0, node1)                                   \
    do {                                                                       \
        ck_assert_uint_eq(node0.port, node1.port);                             \
        ck_assert_str_eq(node0.addr, node1.addr);                              \
        ck_assert_mem_eq(node0.sha, node1.sha, SHA_DIGEST_LENGTH);             \
    } while (0)


