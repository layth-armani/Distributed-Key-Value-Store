#include <errno.h>
#include <stdlib.h>
#include "util.h"
#include "dkvs-client-cmds.h"
#include "network.h"
#include "config.h"

#define STR_PADDING 256

int cli_client_cat(client_t *client, int argc, char **argv){
    if (argv == NULL || argv[0] == NULL || argv[1] == NULL || argc < 2) {
        return ERR_INVALID_COMMAND;
    } 
    if(client == NULL || strlen(argv[0]) > MAX_MSG_ELEM_SIZE || strlen(argv[1]) > MAX_MSG_ELEM_SIZE){
        return ERR_INVALID_ARGUMENT;
    }

    size_t collector_size = STR_PADDING;
    dkvs_value_t collector = calloc(collector_size, sizeof(char));
    dkvs_value_t* fetcher = calloc(1, sizeof(fetcher));
    if (collector == NULL || fetcher == NULL) {
        free(collector);
        free(fetcher);
        return ERR_OUT_OF_MEMORY;
    }
    

    int ret = 0;
    int index = 0;

    while (argv[index+1] != NULL){
        if(strlen(argv[index]) > MAX_MSG_ELEM_SIZE){
            free(collector);
            free(fetcher);
            return ERR_INVALID_ARGUMENT;
        }

        ret = network_get(client, argv[index], fetcher);
        if(ret != ERR_NONE){
            free(collector);
            free(*fetcher);
            free(fetcher);
            return ret;
        }

        if (strlen(collector) + strlen(*fetcher) + 1 >= collector_size) {
            collector_size *= 2;
            char *temp = realloc(collector, collector_size);
            if (temp == NULL) {
                free(collector);
                free(*fetcher);
                free(fetcher);
                return ERR_OUT_OF_MEMORY;
            }
            collector = temp;
        }

        strncat(collector, *fetcher, strlen((*fetcher)));
        free(*fetcher);

        index++;
    }
    collector[strlen(collector)+1] = '\0';

    if(strlen(argv[index]) > MAX_MSG_ELEM_SIZE){
        free(collector);
        free(fetcher);
        return ERR_INVALID_ARGUMENT;
    }
    
    ret = network_put(client, argv[index], collector);

    free(fetcher);
    free(collector);

    return ret;
}

