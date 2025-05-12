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
    M_REQUIRE_NON_NULL(client);
    M_REQUIRE_NON_NULL(key);
    M_REQUIRE_NON_NULL(value);
    if (strlen(key) > MAX_MSG_ELEM_SIZE) return ERR_INVALID_ARGUMENT;

    // to do WEEK 10...
    return ERR_NETWORK;
}

// ======================================================================
static int server_put_send(int fd, struct sockaddr_in server_addr,
                           dkvs_const_key_t key, dkvs_const_value_t value)
{
    M_REQUIRE_NON_NULL(key);
    M_REQUIRE_NON_NULL(value);

    debug_printf("server_put_send(): sending \"%s\" --> \"%s\" to %s:%d\n",
                 key, value,
                 inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

    // to do WEEK 10...
    return ERR_NETWORK;
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
    return ERR_NETWORK;
}
