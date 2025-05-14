/**
 * @file dkvs-server.c
 * @brief server waiting for requests from the DKVS clients
 *
 */

#include "error.h"
#include "config.h" // MAX_MSG_SIZE
#include "hashtable.h"
#include "socket_layer.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HTABLE_SIZE 256

// ======================================================================
static int server_get(int fd, dkvs_const_key_t key,
                      const struct sockaddr_in *client, const Htable_t* table)
{
    M_REQUIRE_NON_NULL(key);
    M_REQUIRE_NON_NULL(client);
    M_REQUIRE_NON_NULL(table);

    debug_printf("server get for key \"%s\"\n", key);

    ssize_t ret = 0;
    // ...to be continued week 11...

    return ret < 0 ? (int) ret : ERR_NONE;
}

// ======================================================================
static int server_put(int fd, dkvs_const_key_t key, dkvs_const_value_t value,
                      const struct sockaddr_in *client, Htable_t* table)
{
    M_REQUIRE_NON_NULL(key);
    M_REQUIRE_NON_NULL(value);
    M_REQUIRE_NON_NULL(client);
    M_REQUIRE_NON_NULL(table);

    debug_printf("server put for \"%s\" --> \"%s\":\n", key, value);

    ssize_t ret = 0;
    // ...to be continued week 11...

    return ret < 0 ? (int) ret : ERR_NONE;
}

// ======================================================================
static int out(int error_code)
{
    if (error_code != ERR_NONE) {
        //fprintf(stderr, "ERROR: %s\n", ERR_MSG(error_code));
    }
    return error_code;
}

// ======================================================================
int main(int argc, char **argv)
{
    int err = ERR_NONE;
    int t = 0;

    // usage: prog <IP> <port> [<key> <value> ...]
    if ((argc < 3) || (argc % 2 == 0)) return out(ERR_INVALID_COMMAND);
    const char* ip = argv[1];

    // --------------- Get port number ---------------
    const char *port_str = argv[2];
    char *endptr;

    // Convert string to unsigned long
    uint16_t port = (uint16_t)strtoul(port_str, &endptr, 10);

    // --------------- Lauch UDP server ---------------
    int fd = udp_server_init(ip, port, t);
    debug_printf("Server listening on %s:%d\n", argv[1], port);

    // --------------- Init Hash table ---------------
    Htable_t* table = Htable_construct(HTABLE_SIZE);
    if (table == NULL) {
        // maybe add something here (or simply remove that comment)...
        return out(ERR_OUT_OF_MEMORY);
    }

    // ...to be continued week 11...
    //perror("Launching Server \n");
    //fprintf(stderr, "Err : %d \n", err);
    char buffer[MAX_MSG_SIZE];
    // --------------- Listening loop ---------------
    while (err == ERR_NONE) {

        struct sockaddr_in address;
        int ret = get_server_addr(ip, port, &address);

        long bytes = udp_read(fd, buffer, MAX_MSG_SIZE, &address);
        fprintf(stderr, "String : %s, strel : %lu, Bytes %d \n", buffer, strlen(buffer), bytes);
        debug_printf("Server listening on %s:%ld\n", ip, port);
        //fprintf(stderr, "Server listening on %s:%d\n", ip, port);


        if (!memchr(buffer, '\0', MAX_MSG_SIZE) == NULL)
        {
            printf("Return of memchr: NO null terminator\n" );
        }
        

        

        if (memchr(buffer, '\0', MAX_MSG_SIZE) == NULL)    
        {
            err = server_put(fd, buffer, buffer + strlen(buffer) + 1, &address, table);
        }
        else if (buffer[0]  == '\0') {
            err = ERR_NOT_FOUND;
        } else {
            err = server_get(fd, buffer, &address, table);
        }

    }

    // --------------- Garbage collecting ---------------
    if (err != ERR_NONE) {
        // maybe add something here (or simply remove that comment)...
        Htable_free(&table);
    }

    return out(err);
}
