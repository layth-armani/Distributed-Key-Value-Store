#include "error.h"
#include "hashtable.h"
#include "network.h"

#define DEFAULT_HASH_TABLE_SIZE 100

static Htable_t *local = NULL;

// ======================================================================
// Fake network content
// ======================================================================
#define ADD_ENTRY(KEY, VALUE)                      \
    do {                                           \
        int ret = network_put(client, KEY, VALUE); \
        if (ret != ERR_NONE)                       \
            return 0;                              \
    } while (0)

#ifndef MAKE_FAKE_NETWORK
#define MAKE_FAKE_NETWORK              \
    do {                               \
        ADD_ENTRY("a", "hello a");     \
        ADD_ENTRY("ab", "hello ab");   \
        ADD_ENTRY("bb", "hello bb");   \
        ADD_ENTRY("bbb", "hello bbb"); \
    } while (0)
#endif

// ======================================================================
// ======================================================================

static int init_if_needed(const client_t *client)
{
    if (local == NULL) {
        puts("==== fake network init ========================");
        local = Htable_construct(DEFAULT_HASH_TABLE_SIZE);
        if (local != NULL) {
            MAKE_FAKE_NETWORK;
        }
        puts("==== end of fake network init =================");
    }
    return (local != NULL);
}

int network_get(const client_t *client, dkvs_const_key_t key, dkvs_value_t *value)
{
    M_REQUIRE_NON_NULL(client);
    M_REQUIRE_NON_NULL(key);
    M_REQUIRE_NON_NULL(value);

    *value = NULL;
    if (init_if_needed(client)) {
        *value = Htable_get_value(local, key);
    }
    return *value == NULL ? ERR_NOT_FOUND : ERR_NONE;
}

int network_put(const client_t *client, dkvs_const_key_t key, dkvs_const_value_t value)
{
    M_REQUIRE_NON_NULL(client);
    M_REQUIRE_NON_NULL(key);
    M_REQUIRE_NON_NULL(value);

    int ret = ERR_NOT_FOUND; // error code if init fails
    if (init_if_needed(client)) {
        ret = Htable_add_value(local, key, value);
    }
    Htable_print(local);
    puts("--------------------------------");

    return ret;
}
