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
    fprintf(stderr,"server_get_send(): asking for key \"%s\" to %s:%d\n",
                 key, inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    // to do WEEK 10...

    int nb_bytes = udp_send(fd, key, strlen(key), &server_addr);

    if (nb_bytes != strlen(key))
    {
        return ERR_NETWORK;
    }
    
    return ERR_NONE;
}

// ======================================================================
static int server_get_recv(int fd, dkvs_value_t* value)
{
    M_REQUIRE_NON_NULL(value);

    // to do WEEK 11...
    debug_printf("server_get_recv(): read \"%s\" (size: %ld)\n", "", -1l);

    return ERR_NETWORK;
}

// ======================================================================
int network_get(const client_t* client, dkvs_const_key_t key, dkvs_key_t* value)
{
    //perror("Entering network get.\n");

    M_REQUIRE_NON_NULL(client);
    M_REQUIRE_NON_NULL(key);
    M_REQUIRE_NON_NULL(value);
    if (strlen(key) > MAX_MSG_ELEM_SIZE) return ERR_INVALID_ARGUMENT;

    node_list_t list = {0 , 0, NULL}; 
   
    int ret = ring_get_nodes_for_key(client->ring, &list, client->ring->size, key);

    if (ret != ERR_NONE)
    {
        node_list_free(&list);
        return ret;
    }

    

    int fd = get_socket(1);

    for (size_t i = 0; i < list.size; i++)
    {
        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &list.nodes[i].addr_s.sin_addr, buf, INET_ADDRSTRLEN);

        int err1 = bind_server(fd, buf,ntohs(list.nodes[i].addr_s.sin_port));

        fprintf(stderr, "Binding to %s:%d\n", buf,ntohs(list.nodes[i].addr_s.sin_port));
        //fprintf(stderr, "%d\n", err1);


        int err2 = server_get_send(fd, list.nodes[i].addr_s, key);
        //fprintf(stderr, "%d\n", err2);


        if (err2 == ERR_NONE)
        {
            node_list_free(&list);
            int err3 = server_get_recv(fd, value);
            //fprintf(stderr, "%d\n", err3);
            return err3;
        }
    }


    node_list_free(&list);
    return ERR_NETWORK;
}

// ======================================================================
static int server_put_send(int fd, struct sockaddr_in server_addr,
                           dkvs_const_key_t key, dkvs_const_value_t value)
{
    M_REQUIRE_NON_NULL(key);
    M_REQUIRE_NON_NULL(value);

    int msg_size = strlen(key) + strlen(value)+1;

    if (msg_size > MAX_MSG_SIZE)
    {
        return ERR_INVALID_ARGUMENT;
    }
    


    debug_printf("server_put_send(): sending \"%s\" --> \"%s\" to %s:%d\n",
                 key, value,
                 inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

    // to do WEEK 10...

    char buffer[msg_size];
    snprintf(buffer, msg_size, "%s\0%s", key, value);

    int nb_bytes = udp_send(fd, buffer, msg_size , &server_addr);
    if (nb_bytes != msg_size)
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
    int ret = ring_get_nodes_for_key(client->ring, &list, client->ring->size, key);

    if (ret != ERR_NONE)
    {
        node_list_free(&list);
        return ret;
    }

    
    int fd = get_socket(1);


    for (size_t i = 0; i < list.size; i++)
    {
        int err1 = bind_server(fd, list.nodes[i].addr,list.nodes[i].port);
        
        int err2 = server_put_send(fd, list.nodes[i].addr_s, key, value);
        
    }
   
    node_list_free(&list);
    return ERR_NETWORK;
}
