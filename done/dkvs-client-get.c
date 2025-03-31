#include <stdlib.h>
#include <string.h>
#include "dkvs-client-cmds.h"
#include "network.h"
#include "config.h"



int cli_client_get(client_t *client, int argc, char **argv){
    if(client == NULL || argc == 0 || argv == NULL || argv[0] == NULL || strlen(argv[0]) > MAX_MSG_ELEM_SIZE){
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

    char** value = malloc(sizeof(dkvs_const_value_t));
    if(value == NULL){
        return ERR_OUT_OF_MEMORY;
    }

    int ret = network_get(client, key, value);
    if (ret == ERR_NONE) printf("OK %s \n", *value);
    else printf("FAIL\n");
    return ret;
}
