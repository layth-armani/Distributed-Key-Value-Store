#include <stdlib.h>
#include <stdlib.h>
#include "client.h"
#include "util.h"

int client_init(client_t *client, size_t supported_args, int *argc, char ***argv){
    
    int arg_err = parse_opt_args(&(client->args),supported_args,argc,argv);
    
    if (arg_err != ERR_NONE)
    {
        return arg_err;
    }
    

    if (client->ring == NULL) {
        client->ring = malloc(sizeof(ring_t));
        if (client->ring == NULL) {
            return ERR_OUT_OF_MEMORY;
        }
    }
    
    return ring_init(client->ring);
}


void client_end(client_t *client){

    if (client !=NULL)
    {
        ring_free(client->ring);
        free(client->ring);
    }
}
