#include <string.h>
#include "util.h"
#include "dkvs-client-cmds.h"
#include "config.h"
#include "network.h"

#define M

int cli_client_find(client_t *client, int argc, char **argv){

    /*ARGUMENT CHECKING*/

    dkvs_const_key_t first_key = NULL;
    dkvs_const_key_t second_key = NULL;

    if (argc != 2 || argv == NULL || argv[0] == NULL)
    {
        return ERR_INVALID_COMMAND;
    }

    if(client == NULL){
        return ERR_INVALID_ARGUMENT;
    }

    size_t first_key_len = strnlen(argv[0], MAX_MSG_ELEM_SIZE);
    strncpy(first_key, argv[0], first_key_len + 1);
    
    if (first_key_len == 0)
    {
        return ERR_INVALID_COMMAND;
    }

    ++argv;
    --argc;

    size_t second_key_len = strnlen(argv[0], MAX_MSG_ELEM_SIZE);
    strncpy(second_key, argv[0],second_key_len + 1);

    if (second_key_len == 0)
    {
        return ERR_INVALID_COMMAND;
    }

    ++argv;
    --argc;

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

    int ret1 = network_get(client,first_key,first_value);
    int ret2 = network_get(client, second_key, second_value);

    if (ret1 == ERR_NOT_FOUND  || ret2 == ERR_NOT_FOUND)
    {
       printf("FAIL\n");
       return ERR_NOT_FOUND; 
    }
    
    
    size_t val2_len = strnlen(second_value, MAX_MSG_ELEM_SIZE);

    char* s = strnstr(first_value,second_value,val2_len);

    if (s == NULL)
    {
        printf("OK -1\n");
    }

    int position = 0;
    int isFound = 0;
   

    while (position < val2_len && !isFound)
    {
        if (&first_value[position] == s)
        {
            printf("OK %d\n", position);
            isFound = 1;
        }
        position++;
    }

    return ERR_NONE;

   
}
