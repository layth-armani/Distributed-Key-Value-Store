#include <stdlib.h>
#include "node.h"
#include "node_list.h"
#include "config.h"

#define NODE_LIST_PADDING 128

node_list_t* node_list_construct(node_list_t* list) {
    if (list == NULL){
        list = malloc(sizeof(node_list_t));
        if (!list)
        {
            fprintf(stderr, "COULDN'T ALLOCATE MEMORY FOR NODE_LIST\n");
            return NULL;
        }
        
    }
    node_list_t result = { 0, 0, NULL };
    result.nodes = calloc(NODE_LIST_PADDING, sizeof(node_t));
    if (result.nodes != NULL) {
        result.allocated = NODE_LIST_PADDING;
    } 
    else {
        return NULL;
    }
    *list = result;
    return list;
}


int get_nodes(node_list_t *nodes){
    if(nodes == NULL || nodes->allocated == 0 || nodes->nodes == NULL){
        nodes = node_list_construct(nodes);
        if(nodes == NULL){
            return ERR_OUT_OF_MEMORY;
        }
        node_t node;
        int test = node_init(&node, DKVS_DEFAULT_IP, DKVS_DEFAULT_PORT, 0);
        if (test != ERR_NONE) return test;
        node_list_add(nodes,node);
    }
    return ERR_NONE; 
}


node_list_t* node_list_enlarge(node_list_t* list) {
    if (list != NULL) {
        node_list_t result = *list;
        result.allocated += NODE_LIST_PADDING;
        if ((result.allocated > SIZE_MAX / sizeof(node_t)) || ((result.nodes = realloc(result.nodes, result.allocated * sizeof(node_t)))== NULL)) {
            return NULL; 
        }
        *list = result;
    }
    return list;
}


int node_list_add(node_list_t *list, node_t node){
    if (list == NULL) {
        fprintf(stderr, "Invalid Arguments for adding to a node list: Returning ERR_INVALID_ARGUMENT\n");
        return ERR_INVALID_ARGUMENT;
    }

    while (list->size >=list->allocated) {
        if (node_list_enlarge(list) == NULL) {
            fprintf(stderr, "Couldn't allocate additional memory to add a node to a node list: Returning ERR_OUT_OF_MEMORY\n");
            return ERR_OUT_OF_MEMORY;
        }
    }
    list->nodes[list->size] = node;
    ++(list->size);
    return ERR_NONE;
}




void node_list_free(node_list_t *list){
    if ((list != NULL) && (list->nodes != NULL)) {

        for (size_t i = 0; i < list->size; i++)
        {
            node_end(list->nodes + i);
        }
        free(list->nodes);

        list->nodes = NULL;
        list->size = 0;
        list->allocated = 0;

        free(list);
    }
}


