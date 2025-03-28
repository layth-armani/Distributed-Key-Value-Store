#pragma once

/**
 * @file node_list.h
 * @brief Parsing and handling of a list of nodes (= list of servers until week
 * 11). This data structure is required only from week 06.
 *
 * @author Valérian Rousset
 */

#include "node.h"

/**
 * @brief list of nodes
 */
/* TODO WEEK 06:
 * Define struct node_list here
 * (and remove these three lines of comment).
 */
typedef struct node_list node_list_t;

/**
 * @brief parse the DKVS_SERVERS_LIST_FILENAME file and return the corresponding
 * list of nodes
 *
 * @param nodes (out) where the newly allocated list will be stored
 * @return an error code (see error.h)
 */
int get_nodes(node_list_t *nodes);

/**
 * @brief add a node to a list of nodes
 * @param list list of nodes where to add to (modified)
 * @param node the node to be added
 * @return some error code indicating whether addition fails or not
 */
int node_list_add(node_list_t *list, node_t node);

/**
 * @brief sort node list according to node comparison function
 * @param list list of nodes to be sorted
 * @param comparator node comparison function
 */
void node_list_sort(node_list_t *list,
                    int (*comparator)(const node_t *, const node_t *));

/**
 * @brief free the given list of nodes
 * @param list list of nodes to clean
 */
void node_list_free(node_list_t *list);
