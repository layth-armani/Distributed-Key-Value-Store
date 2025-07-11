#pragma once

/**
 * @file hashtable.h
 * @brief Local hash-table implementation.
 *
 * @author Valérian Rousset
 */

#include <stddef.h> // for size_t

/*
 * Definition of type for local hash-tables keys
*/

 typedef char* dkvs_key_t;
 typedef const char* dkvs_const_key_t;
 
/*
* Definition of type for local hash-tables values
*/
 
 typedef char* dkvs_value_t;
 typedef const char* dkvs_const_value_t;

 /*
 * key-value pair
 */
/* TODO WEEK 05:
 * Here define the kv_pair_t type
 * (and remove these three lines of then-useless comment).
 */
 typedef struct
 {
    dkvs_key_t key;
    dkvs_value_t value;
 }kv_pair_t;


/*
 * Definition of local hash-table type
 */

struct bckt_t;
typedef struct bckt_t bucket_t;


typedef struct {
    size_t size;
    bucket_t* content;
}Htable_t;


/* TODO WEEK 08:
 * Here define the kv_list_t type
 * (and remove these three lines of then-useless comment).
 */
typedef void kv_list_t;

/**
 * @brief construct a hash-table of the given size.
 * @param size number of buckets in the new hash-table
 * @return the newly allocated hash-table
 */
Htable_t* Htable_construct(size_t size);

/**
 * @brief free the given hash-table pointer (as well as its content).
 * This pointer is assigne to NULL.
 * @param p_table the hash-table pointer to free (passed by reference)
 */
void Htable_free(Htable_t** p_table);

/**
 * @brief free the content of the given hash-table
 * @param table the hash-table to free (passed by reference)
 */
void Htable_free_content(Htable_t* table);

/**
 * @brief add a key:value pair to hash-table
 * @param table the table where to add
 * @param key the key to which the value shall be associated
 * @param value the value to be added
 * @return 0 on success; error code on error (see error.h)
 */
int Htable_add_value(Htable_t* table, dkvs_const_key_t key, dkvs_const_value_t value);

/**
 * @brief get a value for a given in the given hash-table
 * @param table the table where to get
 * @param key the key associated to the wanted value
 * @return the associated value
 */
dkvs_value_t Htable_get_value(const Htable_t* table, dkvs_const_key_t key);

/**
 * @brief compute the hash for the given key and size of hash-table.
 *      Note: although this is a local function, it is exposed here
 *            for test/grading purposes.
 * @param key the key onto compute the hash
 * @param table_size size of the containing table
 * @return a hash value in range [0..table_size-1], or SIZE_MAX if error
 */
size_t hash_function(dkvs_const_key_t key, size_t table_size);

/**
 * @brief get the content of the table
 *    Note: does NOTHING until week 08.
 * @param table table to read from
 * @return the list of key values from the given table, NULL if a failure occured
 */
kv_list_t *Htable_get_content(const Htable_t* table);

/**
 * @brief delete a key:value pair from the hash-table
 *    Note: does NOTHING until week 11.
 * @param table the table where to delete
 * @param key the key which is to be delete
 * @return 0 on success; error code on errror (see error.h)
 */
int Htable_remove_key(Htable_t* table, dkvs_const_key_t key);

/**
 * @brief free a key-value pair content (both of them)
 * @param kv the key-value pair to free
 */
void kv_pair_free(kv_pair_t *kv);

/**
 * @brief free a kv_list and it's content
 *    Note: does NOTHING until week 08.
 * @param list the list to free
 */
void kv_list_free(kv_list_t *list);

/**
 * @brief prints the content of a hashtable
 * @param table the table to be printed
 */
void Htable_print(const Htable_t* table);

/**
 * @brief prints the content of a hashtable in a buffer, starting from index `from`
 * @param table the table to be printed
 * @param from the bucket index to start from (included)
 * @param to   (output) the first bucket index that was NOT included in the print
 *        (i.e. where to continue from in the next round); equal to `from` in case of errors
 * @param buffer the buffer where to write into; it should be of size greater or equal to `buffer_size`
 * @param buffer_size the maximum number of chars to be possibly written into `buffer`
 * @return ERR_NONE on success;
 *         ERR_INVALID_ARGUMENT if buffer is NULL or buffer_size is 0;
 *         ERR_INVALID_CONFIG if cannot write further in buffer (buffer_size too small).
 */
int Htable_dump(const Htable_t* table, size_t from, size_t* to, char* buffer, size_t buffer_size);
