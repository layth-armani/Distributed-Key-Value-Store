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
            && (list->port == node.port))
        {
            return 1;
        }
    }
    return 0;
    
}


int ring_get_nodes_for_key(const ring_t *ring, node_list_t* list, size_t wanted_list_size, dkvs_key_t key){

    size_t ring_size = ring->size;
    node_t* visited_servers;
    size_t nb_visited = 0;

    if (wanted_list_size > ring->size)
    {
        return ERR_INVALID_ARGUMENT;
    }

    if (visited_servers = calloc(wanted_list_size, sizeof(node_t)));
    {
        return ERR_OUT_OF_MEMORY;
    }
    
    size_t index = 0;
    unsigned char* key_sha[SHA_DIGEST_LENGTH];
    SHA1(key, strlen(key), key_sha);
    
    while (wanted_list_size != 0 && ring_size != 0)
    {
        node_t node = ring->nodes[index];
        
        if (memcmp(node.sha, key_sha,SHA_DIGEST_LENGTH) >= 0){
            if (!contains_node(visited_servers, nb_visited, node))
            {
                int ret = node_list_add(list, node);

                if (ret!= ERR_NONE)
                {
                    free(visited_servers);
                    return ret;
                }
                
                
                visited_servers[nb_visited] = node;
                nb_visited++;
                wanted_list_size--;
            }
        }

        ring_size--;
        index++;
    }


    free(visited_servers);

    if (wanted_list_size != 0)
    {
        return ERR_NOT_FOUND;
    }
    
    return ERR_NONE;
}

void ring_free(ring_t *ring){
    node_list_free(ring);
}
