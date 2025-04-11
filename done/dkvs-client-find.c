#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "dkvs-client-cmds.h"
#include "config.h"
#include "network.h"

#define M

int cli_client_find(client_t *client, int argc, char **argv){

    /*ARGUMENT CHECKING*/

    
    if (argc != 2 || argv == NULL || argv[0] == NULL)
    {
        return ERR_INVALID_COMMAND;
    }
    
    if(client == NULL){
        return ERR_INVALID_ARGUMENT;
    }
    
    
    size_t first_key_len = strnlen(argv[0], MAX_MSG_ELEM_SIZE);
    
    if (first_key_len == 0)
    {
        return ERR_INVALID_COMMAND;
    }

    dkvs_key_t first_key = calloc(first_key_len + 1 , sizeof(char));
    
    if (!first_key)
    {
        return ERR_OUT_OF_MEMORY;
    }
    
    strncpy(first_key, argv[0], first_key_len + 1);

    ++argv;
    --argc;
    
    size_t second_key_len = strnlen(argv[0], MAX_MSG_ELEM_SIZE);
    if (second_key_len == 0)
    {
        return ERR_INVALID_COMMAND;
    }
    
    ++argv;
    --argc;
    
    dkvs_key_t second_key = calloc(second_key_len + 1, sizeof(char));
    if (!second_key)
    {
        return ERR_OUT_OF_MEMORY;
    }

    strncpy(second_key,argv[0], second_key_len + 1);
    
    


    dkvs_value_t* first_value = malloc(sizeof(dkvs_value_t));
    
    if (!first_value)
    {
        return ERR_OUT_OF_MEMORY;
    }   

    dkvs_value_t* second_value = malloc(sizeof(dkvs_value_t));
    if (!second_value)
    {
        free(first_value);
        return ERR_OUT_OF_MEMORY;
    }

    int ret1 = network_get(client, first_key,first_value);
    int ret2 = network_get(client, second_key, second_value);

    if (ret1 == ERR_NOT_FOUND  || ret2 == ERR_NOT_FOUND)
    {
       printf("FAIL\n");
       return ERR_NOT_FOUND; 
    }
    
    
    size_t val2_len = strnlen(*second_value, MAX_MSG_ELEM_SIZE);

    char* s = strnstr(*first_value, *second_value, val2_len);

    if (s == NULL)
    {
        printf("OK -1\n");
        return ERR_NONE;
    }

    size_t position = (size_t)(s - *first_value);

    if (position < val2_len)
    {
        printf("OK %zu\n", position);
    }
    else
    {
        printf("OK -1\n");
    }

    return ERR_NONE;

   
}
