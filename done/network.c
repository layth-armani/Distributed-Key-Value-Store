/**
 * @file network.c
 * @brief Implementation of network.h
 *
 */

#include "network.h"
#include "error.h"

#ifdef DEBUG
// for INET address printing (inet_ntoa(() and ntohs())
#include <arpa/inet.h>
#endif

#include <assert.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "args.h"
#include "socket_layer.h"
#include "config.h"
#include "client.h"
#include "node.h"
#include "node_list.h"

// ======================================================================
static int server_get_send(int fd, struct sockaddr_in server_addr, dkvs_const_key_t key)
{
    M_REQUIRE_NON_NULL(key);
    debug_printf("server_get_send(): asking for key \"%s\" to %s:%d\n",
                 key, inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    // to do WEEK 10...

    ssize_t nb_bytes = udp_send(fd, key, strlen(key), &server_addr);

    if (nb_bytes != (ssize_t)strlen(key))
    {
        return ERR_NETWORK;
    }
    
    return ERR_NONE;
}

// ======================================================================
static int server_get_recv(int fd, dkvs_value_t* value)
{
    M_REQUIRE_NON_NULL(value);

    char buffer[MAX_MSG_ELEM_SIZE];
    ssize_t bytes = udp_read(fd, buffer, MAX_MSG_ELEM_SIZE, NULL);
    debug_printf("server_get_recv(): read \"%s\" (size: %ld)\n", "", -1l);

    return ERR_NETWORK;
}

// ======================================================================
int network_get(const client_t* client, dkvs_const_key_t key, dkvs_key_t* value)
{

    M_REQUIRE_NON_NULL(client);
    M_REQUIRE_NON_NULL(key);
    M_REQUIRE_NON_NULL(value);
    if (strlen(key) > MAX_MSG_ELEM_SIZE) return ERR_INVALID_ARGUMENT;

    node_list_t list = {0 , 0, NULL}; 
   
    int err = ring_get_nodes_for_key(client->ring, &list, client->ring->size, key);

    if (err != ERR_NONE)
    {
        node_list_free(&list);
        return err;
    }

    

    int fd = client->socket;

    for (size_t i = 0; i < list.size; i++)
    {
        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &list.nodes[i].addr_s.sin_addr, buf, INET_ADDRSTRLEN);

        err = server_get_send(fd, list.nodes[i].addr_s, key);
        if (err == ERR_NONE)
        {
            node_list_free(&list);
            err = server_get_recv(fd, value);
            return err;
        }
    }


    node_list_free(&list);
    return err;
}

// ======================================================================
static int server_put_send(int fd, struct sockaddr_in server_addr,
                           dkvs_const_key_t key, dkvs_const_value_t value)
{
    M_REQUIRE_NON_NULL(key);
    M_REQUIRE_NON_NULL(value);

    size_t msg_size = strlen(key) + strlen(value) + 1;

    if (msg_size > MAX_MSG_SIZE)
    {
        return ERR_INVALID_ARGUMENT;
    }
    
    debug_printf("server_put_send(): sending \"%s\" --> \"%s\" to %s:%d\n",
                 key, value,
                 inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

    // to do WEEK 10...

    char buffer[msg_size];
    memcpy(buffer, key, strlen(key) + 1);
    memcpy(&buffer[strlen(key)+1] , value, strlen(value));

    ssize_t nb_bytes = udp_send(fd, buffer, msg_size , &server_addr);
    if (nb_bytes != (ssize_t)msg_size)
    {
        return ERR_NETWORK;
    }



    return ERR_NONE;
}

// ======================================================================
int network_put(const client_t* client, dkvs_const_key_t key, dkvs_const_value_t value)
{
    M_REQUIRE_NON_NULL(client);
    M_REQUIRE_NON_NULL(key);
    M_REQUIRE_NON_NULL(value);
    if (strlen(key)   > MAX_MSG_ELEM_SIZE) return ERR_INVALID_ARGUMENT;
    if (strlen(value) > MAX_MSG_ELEM_SIZE) return ERR_INVALID_ARGUMENT;

    // to do WEEK 10...
    node_list_t list = {0, 0 , NULL};
    int err = ring_get_nodes_for_key(client->ring, &list, client->ring->size, key);

    if (err != ERR_NONE)
    {
        node_list_free(&list);
        return err;
    }

    
    int fd = client->socket;
    int ret = err;

    for (size_t i = 0; i < list.size; i++)
    {
        err = server_put_send(fd, list.nodes[i].addr_s, key, value);
        if(err != ERR_NONE)ret = err;
    }
   
    node_list_free(&list);
    return ret;
}
