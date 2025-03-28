#include <openssl/sha.h>
#include <string.h>
#include <stdlib.h>
#include "hashtable.h"
#include "error.h"
#include "node.h"

int node_init(node_t *node, const char *ip, uint16_t port, size_t node_id){
    if (node == NULL || ip == NULL || strlen(ip) == 0){
        fprintf(stderr, "Invalid Arguments for initialization of a node: Returning ERR_INVALID_ARGUMENT\n");
        return ERR_INVALID_ARGUMENT;
    }
    char* address = NULL;
    if((address = calloc(strlen(ip)+1, sizeof(char))) == NULL){
        fprintf(stderr, "Couldn't allocate memory for the Node Address: Returning ERR_OUT_OF_MEMORY\n");
        return ERR_OUT_OF_MEMORY;
    }
    strncpy(address,ip,strlen(ip)+1);
    node->address = address;
    return ERR_NONE;
}

void node_end(node_t *node){
    return;
}

