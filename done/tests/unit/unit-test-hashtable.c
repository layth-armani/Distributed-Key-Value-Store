/**
 * @file unit-test-hashtable.c
 * @brief test code for hashtables
 *
 * @author Valérian Rousset & Jean-Cédric Chappelier
 */
#pragma GCC diagnostic ignored "-Wunused-function"

#include <stdio.h> // for puts(). to be removed when no longer needed.

#include <check.h>
#include <time.h>

#include "./test.h"
#include "../../util.h"
#include "../../error.h"
#include "../../hashtable.h"

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

void print_global_ht(void){
    Htable_print(global_table);
}

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
            //printf("TEST FOR (%s,%s)\n",*key,*value);
            ck_assert_err_none(Htable_add_value(global_table, *key,  *value));
            ck_assert_get_value_eq(global_table, *key, *value);
        }
    }
    
   
}
END_TEST

// ======================================================================
START_TEST(test_full_table)
{
    #define KEY_LEN 3
    #define VAL_LEN 6

    char alphabet[26] = "abcdefghijklmnopqrstuvwxyz";
    dkvs_key_t key = malloc(KEY_LEN*sizeof(char));
    dkvs_key_t value = malloc(VAL_LEN*sizeof(char));


    for (size_t i = 0; i < global_table->size; i++)
    {
        for (size_t j = 0; j < KEY_LEN; j++)
        {   
            char c = RANDOM(alphabet);
            key[j] = c;
        }
        key[KEY_LEN - 1] = '\0';

        for (size_t k = 0; k < VAL_LEN; k++)
        {
            char c = RANDOM(alphabet);
            value[k] = c;
        }
        value[VAL_LEN -1] = '\0';

        ck_assert_err_none(Htable_add_value(global_table,key,value));
        ck_assert_get_value_eq(global_table,key,value);

    }
    free(key);
    free(value);

    //Htable_print(global_table);
    
    
}
END_TEST

// ======================================================================
START_TEST(construct_edges)
{
    Htable_t* zero_size = Htable_construct(0);
    ck_assert_ptr_null(zero_size);
    Htable_t* large_size = Htable_construct((SIZE_MAX));
    ck_assert_ptr_null(zero_size);
}
END_TEST

// ======================================================================
START_TEST(free_edges)
{
    Htable_t* n = NULL;
    Htable_t** nu = NULL;
    Htable_t** nn = malloc(sizeof(Htable_t*));
    *nn = n;

    Htable_t* ht = Htable_construct(1);
    Htable_t** htht = malloc(sizeof(Htable_t*));
    *htht = ht;

    Htable_free_content(n);
    Htable_free_content(ht);

    Htable_free(nu);
    Htable_free(nn);
    free(nn);
    Htable_free(htht);
    free(htht);

    ck_assert_int_eq(0,0);
}
END_TEST

// ======================================================================

START_TEST(add_edges)
{
    #define KEY_LEN 3
    #define VAL_LEN 6

    char alphabet[26] = "abcdefghijklmnopqrstuvwxyz";
    dkvs_key_t key = malloc(KEY_LEN*sizeof(char));
    dkvs_key_t value = malloc(VAL_LEN*sizeof(char));


    for (size_t i = 0; i < global_table->size; i++)
    {
        for (size_t j = 0; j < KEY_LEN; j++)
        {   
            char c = RANDOM(alphabet);
            key[j] = c;
        }
        key[KEY_LEN - 1] = '\0';

        for (size_t k = 0; k < VAL_LEN; k++)
        {
            char c = RANDOM(alphabet);
            value[k] = c;
        }
        value[VAL_LEN -1] = '\0';

        ck_assert_err_none(Htable_add_value(global_table,key,value));
        ck_assert_get_value_eq(global_table,key,value);

    }
    free(key);
    free(value);
    ck_assert_err_none(Htable_add_value(global_table, "",  ""));
    ck_assert_get_value_eq(global_table, "", "");

    ck_assert_err_none(Htable_add_value(global_table, "\0",  "\0"));
    ck_assert_get_value_eq(global_table, "\0", "\0");

    const char* big_s = calloc(1, 0x100000000/sizeof(char));

    ck_assert_err_none(Htable_add_value(global_table, big_s,  big_s));
    ck_assert_get_value_eq(global_table, big_s, big_s);

    ck_assert_invalid_arg(Htable_add_value(NULL, big_s,  big_s));
    ck_assert_invalid_arg(Htable_add_value(global_table, big_s,  NULL));
    ck_assert_invalid_arg(Htable_add_value(NULL, NULL,  big_s));

    ck_assert_invalid_arg(Htable_add_value(global_table, NULL,  big_s));
    ck_assert_ptr_null(Htable_get_value(global_table, NULL));



    free((void*)big_s);
}
END_TEST

// ======================================================================
START_TEST(stress_test_add_get)
{
    srand(time(NULL)); // Needs '#include <time.h>' to work.

    #define NUM_ITERATIONS 10000 
    #define MAX_KEY_LEN 15
    #define MAX_VAL_LEN 20
    char alphabet[27] = "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < NUM_ITERATIONS; ++i)
    {
        size_t key_len = (rand() % MAX_KEY_LEN) + 1;
        dkvs_key_t key = malloc(key_len * sizeof(char));
        ck_assert_ptr_nonnull(key);
        for (size_t j = 0; j < key_len - 1; ++j)
        {
            key[j] = RANDOM(alphabet);
        }
        key[key_len - 1] = '\0';

        size_t val_len = (rand() % MAX_VAL_LEN) + 1;
        dkvs_value_t value = malloc(val_len * sizeof(char));
        ck_assert_ptr_nonnull(value);
        for (size_t k = 0; k < val_len - 1; ++k)
        {
            value[k] = RANDOM(alphabet);
        }
        value[val_len - 1] = '\0';

        ck_assert_err_none(Htable_add_value(global_table, key, value));

        ck_assert_get_value_eq(global_table, key, value);

        free(key);
        free(value);
    }

    
    FOREACH_KEY_PTR(key_ptr) {
        FOREACH_VALUE_PTR(value_ptr) {
            ck_assert_err_none(Htable_add_value(global_table, *key_ptr, *value_ptr));
            ck_assert_get_value_eq(global_table, *key_ptr, *value_ptr);
        }
    }
    //Htable_print(global_table);
}
END_TEST

// ======================================================================
Suite *hashtable_suite(void)
{

    Suite *s = suite_create("Tests for hashtable.c");


    Add_Test(s, construct_and_delete);

    Add_Test_With_Fixture(s, add_value_does_retrieve_same_value, allocate_global_ht, free_global_ht);

    Add_Test_With_Fixture(s,test_full_table,allocate_global_ht,free_global_ht);

    Add_Test_With_Fixture(s,construct_edges,allocate_global_ht,free_global_ht);

    Add_Test_With_Fixture(s,free_edges,allocate_global_ht,free_global_ht);

    Add_Test_With_Fixture(s,add_edges,allocate_global_ht,free_global_ht);

    Add_Test_With_Fixture(s,stress_test_add_get,allocate_global_ht,free_global_ht);




    

    return s;
}

TEST_SUITE(hashtable_suite)
