#include <openssl/sha.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include "hashtable.h"
#include "error.h"
#include "node.h"
#include "util.h"

#define STRING_LENGTH_SHA 40


int node_init(node_t *node, const char *ip, uint16_t port, size_t node_id){
    if (node == NULL || ip == NULL || strlen(ip) == 0){
        fprintf(stderr, "Invalid Arguments for initialization of a node: Returning ERR_INVALID_ARGUMENT\n");
        return ERR_INVALID_ARGUMENT;
    }



    struct sockaddr_in server_addr;
    zero_init_var(server_addr);
    int err = get_server_addr(ip, port, &server_addr);

    if(err!= ERR_NONE){
        return ERR_ADDRESS;
    }

    node->addr_s = server_addr;
    


    char* address = NULL;
    if((address = calloc(strlen(ip)+1, sizeof(char))) == NULL){
        fprintf(stderr, "Couldn't allocate memory for the Node Address: Returning ERR_OUT_OF_MEMORY\n");
        return ERR_OUT_OF_MEMORY;
    }
    strncpy(address,ip,strlen(ip)+1);
    node->addr = address;
    node->port = port;
    

    
    char* string_to_hash = calloc(STRING_LENGTH_SHA, sizeof(char));
    if (string_to_hash == NULL)
    {
        free(address);
        return ERR_OUT_OF_MEMORY;
    }
    unsigned char* sha = calloc(SHA_DIGEST_LENGTH, sizeof(char));
    if (string_to_hash == NULL)
    {
        free(address);
        free(string_to_hash);
        return ERR_OUT_OF_MEMORY;
    }


    //Create string to be hashed
    snprintf(string_to_hash, STRING_LENGTH_SHA, "%s %hu %zu", ip, port, node_id);

    SHA1( (unsigned char*) string_to_hash, strlen(string_to_hash), sha);

    node->sha = sha;
    free(string_to_hash);

    return ERR_NONE;
}

void node_end(node_t *node){
    if(node != NULL) {
        if (node->addr != NULL) {
            free((void*)node->addr);
            node->addr = NULL;
        }
        if (node->sha != NULL) {
            free(node->sha);
            node->sha = NULL;
        }
    }
    return;
}

int node_cmp_sha(const node_t *first, const node_t *second){
    if (first == NULL || second == NULL)
    {
        return 0;
    }
    
    return memcmp(first->sha,second->sha,SHA_DIGEST_LENGTH);
}


