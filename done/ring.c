#include <stdlib.h>
#include "ring.h"
#include "node_list.h"

int ring_init(ring_t *ring){

    ring->allocated = 0;
    ring->size = 0;
    ring->nodes = NULL;

    return get_nodes(ring);
}

void ring_free(ring_t *ring){
    node_list_free(ring);
    free(ring);
}
