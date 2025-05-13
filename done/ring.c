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
        return i;
    }

    node_list_sort(ring, node_cmp_sha);

    return ERR_NONE;

}

int contains_node(node_list_t* list ,node_t* node){

    if (list->size == 0)
    {
        return 0;
    }

    for (size_t i = 0; i < list->size ; i++)
    {
        struct sockaddr_in* a = &list->nodes[i].addr_s;
        struct sockaddr_in* b = &node->addr_s;

        if (a->sin_family == b->sin_family &&
            a->sin_port == b->sin_port &&
            a->sin_addr.s_addr == b->sin_addr.s_addr) {
            return 1;
        }
    }
    return 0;
}


int ring_get_nodes_for_key(const ring_t *ring, node_list_t* list, size_t wanted_list_size, dkvs_const_key_t key){

    puts("RING");
    node_list_print(ring);
    puts("END RING");
    size_t index = 0;
    size_t first_index = -1;

    unsigned char key_sha[SHA_DIGEST_LENGTH];

    SHA1((unsigned char*)key, strlen(key), key_sha);


    // Find first node in the ring with a bigger or equal SHA

    while(first_index == -1 && index < ring->size)
    {
        if (memcmp(ring->nodes[index].sha, key_sha, SHA_DIGEST_LENGTH) >= 0)
        {
            first_index = index;

        }
        index++;
    }

    first_index = (first_index == -1) ? 0 : first_index;

    // Add to the list the nodes
    
    size_t ring_index = first_index;
    size_t total_nodes = ring->size;
    
    while (wanted_list_size != 0 && total_nodes != 0)
    {

        ring_index = ring_index % ring->size;
        node_t* node = &ring->nodes[ring_index];
        
        
        if (!contains_node(list, node))
        {
            int ret = node_list_add(list, *node);

            if (ret != ERR_NONE)
            {
                return ret;
            }

            wanted_list_size--;
        }

        total_nodes--;
        ring_index++;
    }

    puts("LIST");
    node_list_print(list);
    puts("END LIST");
  
    return ERR_NONE;
}

void ring_free(ring_t *ring){
    node_list_free(ring);
}


