#include <stdlib.h>
#include "ring.h"
#include "node_list.h"
#include <string.h>

int ring_init(ring_t *ring){

    ring->allocated = 0;
    ring->size = 0;
    ring->nodes = NULL;

    int i = get_nodes(ring);
    
    if (i != ERR_NONE){
        fprintf(stderr, "Error in get_nodes call: %s\n", ERR_MSG(i));
    }

    node_list_sort(ring, node_cmp_sha);

    return ERR_NONE;

}

int contains_node(node_t* list,size_t size ,node_t node){

    if (size == 0)
    {
        return 0;
    }

    for (size_t i = 0; i < size ; i++)
    {
        if (!strncmp(list[i].addr,node.addr,strlen(list[i].addr)) 
            && (list[i].port == node.port))
        {
            printf("Is the same server\n");
            return 1;
            
        }
    }
    return 0;
}


int ring_get_nodes_for_key(const ring_t *ring, node_list_t* list, size_t wanted_list_size, dkvs_const_key_t key){

    size_t ring_size = ring->size;

    
    size_t index = 0;
    unsigned char key_sha[SHA_DIGEST_LENGTH];
    
    SHA1((unsigned char*)key, strlen(key), key_sha);
    
    while (wanted_list_size != 0 && ring_size != 0)
    {
        node_t node = ring->nodes[index];
        
        if (memcmp(node.sha, key_sha,SHA_DIGEST_LENGTH) >= 0){
            if (!contains_node(list->nodes, list->size, node))
            {

                node_t node_copy;


                node_copy.addr = strndup(node.addr, strlen(node.addr) + 1);
                node_copy.port = node.port;

                node_copy.sha = malloc(SHA_DIGEST_LENGTH);
                if (node_copy.sha == NULL) {
                    free((void*)node_copy.addr);
                    return ERR_OUT_OF_MEMORY;
                }
                memcpy(node_copy.sha, node.sha, SHA_DIGEST_LENGTH);


                int ret = node_list_add(list, node_copy);

                if (ret!= ERR_NONE)
                {
                    return ret;
                }
            
                wanted_list_size--;
            }
        }

        ring_size--;
        index++;
    }
    return ERR_NONE;
}

void ring_free(ring_t *ring){
    node_list_free(ring);
}


