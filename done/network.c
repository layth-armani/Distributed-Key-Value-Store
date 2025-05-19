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
#include <sys/time.h>
#include <string.h>

#include "args.h"
#include "socket_layer.h"
#include "config.h"
#include "client.h"
#include "node.h"
#include "node_list.h"
#include "util.h"


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

    char* buffer = calloc(MAX_MSG_ELEM_SIZE, sizeof(char));
    if (!buffer) return ERR_OUT_OF_MEMORY;
     
    ssize_t bytes = udp_read(fd, buffer, MAX_MSG_ELEM_SIZE, NULL);
    debug_printf("server_get_recv(): read \"%s\" (size: %ld)\n", buffer, bytes);

    if (bytes == 1 && buffer[0] == '\0')
    {
        free(buffer);
        return ERR_NOT_FOUND;
    }
    else if (memchr(buffer, '\0', (size_t)bytes) != NULL){
        free(buffer);
        return ERR_NETWORK;
    }
    else if (bytes >= 0){
        *value = calloc((size_t) bytes +1, sizeof(char));
        if(!value){
            free(buffer);
            return ERR_OUT_OF_MEMORY;
        }

        memcpy(*value, buffer, (size_t)bytes);
        (*value)[bytes] = '\0';
        free(buffer);
        return ERR_NONE;
    }
    free(buffer);
    return ERR_NETWORK;
}

// ======================================================================
int network_get(const client_t* client, dkvs_const_key_t key, dkvs_value_t* value)
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
    
    
    Htable_t* count_table = Htable_construct(256);
    size_t R = client->args.get_needed;

    int fd = client->socket;

   



    for (size_t i = 0; i < MIN(list.size, client->args.total_servers) ; i++)
    {
      
        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &list.nodes[i].addr_s.sin_addr, buf, INET_ADDRSTRLEN);


        err = server_get_send(fd, list.nodes[i].addr_s, key);        
        if (err == ERR_NONE)
        {
        
            err = server_get_recv(fd, value);
            

            if (err == ERR_NONE)
            {

               
                dkvs_value_t response = Htable_get_value(count_table, *value);
                
                if (!response)
                {
                    response = calloc(2,sizeof(char));
                    if (!response)
                    {
                        Htable_free(&count_table);
                        return ERR_OUT_OF_MEMORY;
                    }
                    response[0] = '1';
                    response[1] = '\0';
                    err = Htable_add_value(count_table, *value, response);
                    
                }
                else{

                    int count = atoi(response);
                    count++;

                    snprintf(response, 2 ,"%d" , count);
                    err = Htable_add_value(count_table, *value, response);
                }

        

                if ((size_t) atoi(response) == R)
                {
                    free(response);
                    node_list_free(&list);
                    Htable_free(&count_table);
                    return err;
                }

                free(response);
                
            }
            
            if (i < MIN(list.size, client->args.total_servers) - 1)
            {
                free(*value);
                *value = NULL;
            }
            
        }
    }

    Htable_free(&count_table);
    node_list_free(&list);
    return ERR_NETWORK;
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

    node_list_t list = {0, 0 , NULL};
    int err = ring_get_nodes_for_key(client->ring, &list, client->ring->size, key);

    if (err != ERR_NONE)
    {
        node_list_free(&list);
        return err;
    }

    int fd = client->socket;
    
    int any_failed = 0;
    size_t succeeded = 0;

    for (size_t i = 0; i < list.size; i++)
    {
        err = server_put_send(fd, list.nodes[i].addr_s, key, value);

        if (err != ERR_NONE) {
            any_failed = 1;
            continue;
        }

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        struct timeval timeout;
        timeout.tv_sec = 1;  
        timeout.tv_usec = 0;

        int sel = select(fd + 1, &readfds, NULL, NULL, &timeout);
        if (sel > 0 && FD_ISSET(fd, &readfds)) {
            char ack[2] = {0};
            ssize_t ack_bytes = udp_read(fd, ack, sizeof(ack), NULL);
            if (ack_bytes != 1 || ack[0] != '\0') {
                any_failed = 1;
            }
            else {
                succeeded++;
                if (succeeded == client->args.put_needed)
                {
                    node_list_free(&list);
                    return ERR_NONE;
                }
                
            }
        } else {
            any_failed = 1;
        }
    }

    node_list_free(&list);
    if(succeeded < client->args.put_needed) return ERR_NETWORK;
    return any_failed ? ERR_NETWORK : ERR_NONE;
}
