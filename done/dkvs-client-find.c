#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "dkvs-client-cmds.h"
#include "config.h"
#include "network.h"

#define M

int cli_client_find(client_t *client, int argc, char **argv) {
    // Validate arguments
    if (argc != 2 || !argv || !argv[0] || !argv[1]) {
        return ERR_INVALID_COMMAND;
    }
    if (!client) {
        return ERR_INVALID_ARGUMENT;
    }

    dkvs_key_t first_key = argv[0];
    dkvs_key_t second_key = argv[1];

    
    dkvs_value_t *first_value = calloc(1, sizeof(dkvs_value_t));
    dkvs_value_t *second_value = calloc(1, sizeof(dkvs_value_t));
    if (!first_value || !second_value) {
        free(first_value);
        free(second_value);
        return ERR_OUT_OF_MEMORY;
    }

    
    int ret1 = network_get(client, first_key, first_value);
    int ret2 = network_get(client, second_key, second_value);

    if (ret1 == ERR_NONE && ret2 == ERR_NONE) {
        size_t val2_len = strnlen(*second_value, MAX_MSG_ELEM_SIZE);
        char *substring = strnstr(*first_value, *second_value, val2_len);

        if (substring) {
            size_t position = (size_t)(substring - *first_value);
            printf("OK %zu\n", position);
        } else {
            printf("OK -1\n");
        }
    } else if (ret1 == ERR_NOT_FOUND || ret2 == ERR_NOT_FOUND) {
        printf("FAIL\n");
        free(*first_value);
        free(*second_value);
        free(first_value);
        free(second_value);
        return ERR_NOT_FOUND;
    }

    
    free(*first_value);
    free(*second_value);
    free(first_value);
    free(second_value);

    return ERR_NONE;
}
