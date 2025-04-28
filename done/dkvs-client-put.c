#include <stdlib.h>
#include <string.h>
#include "dkvs-client-cmds.h"
#include "network.h"
#include "config.h"



int cli_client_put(client_t *client, int argc, char **argv){  
    if (argv == NULL || argv[0] == NULL || argv[1] == NULL || argc != 2) {

        return ERR_INVALID_COMMAND;
    }

    if(client == NULL || strlen(argv[0]) > MAX_MSG_ELEM_SIZE || strlen(argv[1]) > MAX_MSG_ELEM_SIZE){
        return ERR_INVALID_ARGUMENT;
    }
    
    dkvs_const_key_t key = NULL;
    char* key_copy = calloc(strlen(argv[0])+1, sizeof(char));
    if(key_copy==NULL){
        return ERR_OUT_OF_MEMORY;
    }
    strncpy(key_copy,argv[0], strlen(argv[0])+1);
    key = key_copy;

    argv++;
    argc--;

    dkvs_const_value_t value = NULL;
    char* value_copy = calloc(strlen(argv[0])+1, sizeof(char));
    if(value_copy==NULL){
        free(key);
        return ERR_OUT_OF_MEMORY;
    }
    strncpy(value_copy,argv[0], strlen(argv[0])+1);
    value = value_copy;

    int ret = network_put(client, key, value);
    if (ret == ERR_NONE)printf("OK\n");
    else printf("FAIL\n");
    free(key);
    free(value);
    return ret;
}
