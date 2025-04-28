#include <stdlib.h>
#include <errno.h>
#include "node.h"
#include "node_list.h"
#include "config.h"

#define NODE_LIST_PADDING 128


int node_list_server_init(node_list_t* nodes){
    FILE* file = fopen(DKVS_SERVERS_LIST_FILENAME, "r");
    if(file == NULL)return ERR_IO;

    fseek(file, 0, SEEK_END);
    long f_s = ftell(file);
    if(f_s == -1){
        fclose(file);
        return ERR_IO;
    }
    size_t file_size = (size_t) f_s;
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        fclose(file);
        return ERR_OUT_OF_MEMORY;
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';
    fclose(file);

    char* line  = strtok(buffer, "\n");
    while (line != NULL) {
        char *ip = strtok(line, " ");
        char *port_str = strtok(NULL, " ");
        char *id_str = strtok(NULL, " ");

        if(ip == NULL || port_str == NULL ||id_str == NULL || strlen(ip) > 16 || port_str[0] == '-' || id_str[0] == '-'){
            free(buffer);
            return ERR_INVALID_CONFIG;
        }
        
        uint16_t port = atouint16(port_str);
        if(errno != 0){
            free(buffer);
            return ERR_INVALID_CONFIG;
        }
        size_t id = (size_t)atouint64(id_str);
        if(errno != 0){
            free(buffer);
            return ERR_INVALID_CONFIG;
        }

        for (size_t i = 1; i <= id; i++){
            node_t node;
            int ret = node_init(&node, ip, port, id);

            if (ret != ERR_NONE){
                free(buffer);
                return ret;
            }
            node_list_add(nodes,node);
        }
        
        line = strtok(NULL, "\n");
    }

    free(buffer);

    return ERR_NONE;
}



node_list_t* node_list_construct(node_list_t* list) {

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

    if(nodes == NULL) return ERR_INVALID_ARGUMENT;

    if(nodes->allocated == 0 || nodes->nodes == NULL){

        nodes = node_list_construct(nodes);

        if(nodes == NULL){
            return ERR_OUT_OF_MEMORY;
        }
    }

    node_list_server_init(nodes);

    return ERR_NONE; 
}


node_list_t* node_list_enlarge(node_list_t* list) {
    if (list != NULL) {

        node_list_t result = *list;
        result.allocated += NODE_LIST_PADDING;

        if ((result.allocated > SIZE_MAX / sizeof(node_t)) || 
            ((result.nodes = realloc(result.nodes, result.allocated * sizeof(node_t))) == NULL)) 
        {
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

    while (list->size >= list->allocated) {
        if (node_list_enlarge(list) == NULL) {
            fprintf(stderr, "Couldn't allocate additional memory to add a node to a node list: Returning ERR_OUT_OF_MEMORY\n");
            return ERR_OUT_OF_MEMORY;
        }
    }
    
    list->nodes[list->size] = node;
    ++(list->size);
    return ERR_NONE;
}

void node_list_sort(node_list_t *list, int (*comparator)(const node_t *, const node_t *)){
    qsort(list->nodes, list->size, sizeof(node_t), (int (*)(const void *, const void *))comparator);
}


void node_list_free(node_list_t *list){

    if (list == NULL) {
        return; 
    }   

    if (list->nodes != NULL) {
        for (size_t i = 0; i < list->size; i++) {
            node_t *f = list->nodes + i;
            node_end(f);
        }
        free(list->nodes);
        list->nodes = NULL;
    }
}

void node_list_print(const node_list_t *list)
{
    if (list == NULL)
        return;

    for (size_t i = 0; i < list->size; ++i)
    {
        const node_t *const node = list->nodes + i;

        for (size_t j = 0; j < SHA_DIGEST_LENGTH; ++j)
        {
            printf("%02x", node->sha[j]);
        }

        printf(" (%s %d)\n", node->addr, node->port);
    }
}
