/**
 * @file unit-test-hashtable.c
 * @brief test code for hashtables
 *
 * @author Valérian Rousset & Jean-Cédric Chappelier
 */
#pragma GCC diagnostic ignored "-Wunused-function"

#include <stdio.h> // for puts(). to be removed when no longer needed.

#include <check.h>

#include "test.h"
#include "util.h"
#include "hashtable.h"

// ------------------------------------------------------------
#define SIZE(X) (sizeof(X) / sizeof(*(X)))
#define END(X) ((X) + SIZE(X))
#define RANDOM(X) ((X)[((unsigned) rand()) % SIZE(X)])

// ------------------------------------------------------------
static dkvs_key_t KEYS[] = {
    "", "a", "we can put anything in there, it should work"
};

static dkvs_value_t VALUES[] = {
    "", "a", "another stupid value"
};

// ------------------------------------------------------------
#define init_table(T) memset(T, (int) 0xdeadbeef, HTABLE_SIZE * sizeof(dkvs_value_t))

#define FOREACH_KEY_PTR(var)   for (dkvs_key_t   *var = KEYS  ; var < END(KEYS)  ; ++var)
#define FOREACH_VALUE_PTR(var) for (dkvs_value_t *var = VALUES; var < END(VALUES); ++var)

#define ck_assert_get_value_eq(table, key, expected)              \
    do {                                                          \
        const dkvs_value_t actual = Htable_get_value(table, key); \
        if (expected == NULL)                                     \
            ck_assert_ptr_null(actual);                           \
        else                                                      \
            ck_assert_str_eq(actual, expected);                   \
        free(actual);                                             \
    } while (0)

static dkvs_value_t other_value(dkvs_value_t v _unused)
{
    return "string only found here, so this is 'other' enough";
}

// ======================================================================
// shared global variable available for tests in `ht_main` group (see hashtable_suite())
Htable_t* global_table = NULL;

void allocate_global_ht(void)
{ global_table = Htable_construct(256); }

void free_global_ht(void)
{ Htable_free(&global_table); }

// ======================================================================
START_TEST(construct_and_delete)
{
    for (size_t size = 1; size < 3; ++size) {
        Htable_t* table = Htable_construct(size);

        ck_assert_ptr_nonnull(table);

        Htable_free(&table);
    }
}
END_TEST

// ======================================================================
START_TEST(add_value_does_retrieve_same_value)
{
  /* Notice that this test is making use of the global variable `global_table`.
   * It thus has to be in `ht_main` group (see hashtable_suite()).
   */

    FOREACH_KEY_PTR(key) {
        FOREACH_VALUE_PTR(value) {
            ck_assert_err_none(Htable_add_value(global_table, *key,  *value));
            ck_assert_get_value_eq(global_table, *key, *value);
        }
    }
}
END_TEST

// ======================================================================
START_TEST(your_own_test)
{
    puts("Ecrivez ici vos tests et SUPPRIMEZ ce puts");
}
END_TEST

// ======================================================================
Suite *hashtable_suite(void)
{

    Suite *s = suite_create("Tests for hashtable.c");

    Add_Test(s, construct_and_delete);

    Add_Test_With_Fixture(s, add_value_does_retrieve_same_value, allocate_global_ht, free_global_ht);

    return s;
}

TEST_SUITE(hashtable_suite)
