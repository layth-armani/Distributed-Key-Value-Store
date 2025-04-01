#include <stdlib.h>
#include <string.h>
#include "dkvs-client-cmds.h"
#include "network.h"
#include "config.h"



int cli_client_get(client_t *client, int argc, char **argv){
    if (argv == NULL || argv[0] == NULL)
    {
        printf("FAIL, ERROR %s  %u \n", ERR_MSG(ERR_INVALID_COMMAND), ERR_INVALID_COMMAND&255);

        return ERR_INVALID_COMMAND;
    }
    
    if(client == NULL || argc == 0 || strlen(argv[0]) > MAX_MSG_ELEM_SIZE){
        printf("Reached line 11 \n");
        printf("argc: %d \n",argc);
        printf("argv[0]: %s \n", argv[0]);
        printf("argv[1]: %s \n", argv[1]);
        printf("argv[2]: %s \n", argv[2]);
        printf("argv[3]: %s \n", argv[3]);
        printf("argv[4]: %s \n", argv[4]);
        printf("argv[5]: %s \n", argv[5]);
        printf("FAIL, ERROR %s\n", ERR_MSG(ERR_INVALID_ARGUMENT));
        return ERR_INVALID_ARGUMENT;
    }
    printf("Reached line 16 \n");
    printf("argc: %d \n",argc);
    printf("argv[0]: %s \n", argv[0]);
    printf("argv[1]: %s \n", argv[1]);
    printf("argv[2]: %s \n", argv[2]);
    printf("argv[3]: %s \n", argv[3]);
    printf("argv[4]: %s \n", argv[4]);
    printf("argv[5]: %s \n", argv[5]);


    dkvs_const_key_t key = NULL;
    char* key_copy = calloc(strlen(argv[0])+1, sizeof(char));
    if(key_copy==NULL){
        printf("FAIL, ERROR %s\n", ERR_MSG(ERR_OUT_OF_MEMORY));
        return ERR_OUT_OF_MEMORY;
    }
    strncpy(key_copy,argv[0], strlen(argv[0])+1);
    key = key_copy;
    printf("Reached line 23 \n");

    argv++;
    argc--;

    dkvs_value_t* value = calloc(1, sizeof(value));
    if(value == NULL){
        free((void*)key_copy);
        printf("FAIL, ERROR %s\n", ERR_MSG(ERR_OUT_OF_MEMORY));
        return ERR_OUT_OF_MEMORY;
    }

    int ret = network_get(client, key, value);
    if (ret == ERR_NONE) printf("OK %s \n", *value);
    else printf("FAIL, ERROR %s\n", ERR_MSG(ret));
    free((void*)key_copy);
    free((void*)*value);
    free((void*)value);
    return ret;
}
