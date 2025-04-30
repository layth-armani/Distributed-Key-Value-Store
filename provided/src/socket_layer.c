/*
 * @file socket_layer.c
 * @brief socket binding for CS-202 project
 *
 */

#include "socket_layer.h"

#include "error.h"
#include "util.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// ======================================================================
int get_socket(time_t t)
{
    const int fd = -1;

    if (t > 0) {
        // Set receive timeout
        struct timeval timeout;
        zero_init_var(timeout);
        // ...to be continued...
    }

    return fd;
}

// ======================================================================
int get_server_addr(const char *ip, uint16_t port,
                    struct sockaddr_in *p_server_addr)
{
    M_REQUIRE_NON_NULL(ip);
    M_REQUIRE_NON_NULL(p_server_addr);

    struct sockaddr_in server_addr;
    zero_init_var(server_addr);

        // ...to be continued...

    return ERR_NONE;
}

// ======================================================================
int bind_server(int socket, const char *ip, uint16_t port)
{
    struct sockaddr_in server_addr;
    zero_init_var(server_addr);

    int err = ERR_NONE;
    // ...to be continued...

    return err;
}

// ======================================================================
int udp_server_init(const char *ip, uint16_t port, time_t t)
{
    M_REQUIRE_NON_NULL(ip);

    // Create a socket
    const int socket = -1; // to be modified
    if (socket < 0) {
        perror("Error opening socket");
        return ERR_NETWORK;
    }

    // Bind the socket to the address
    // ...to be continued...

    return socket;
}

/********************************************************************/
ssize_t udp_read(int socket, char *buf, size_t buflen,
                 struct sockaddr_in *cli_addr)
{
    M_REQUIRE_NON_NULL(buf);

    // ...to be continued...
    return ERR_NETWORK; // remove that line if needed
}

/********************************************************************/
ssize_t udp_send(int socket, const char *response, size_t response_len,
                 const struct sockaddr_in *cli_addr)
{
    M_REQUIRE_NON_NULL(response);

    const ssize_t res = -1; // to be modified

    return res < 0 ? ERR_NETWORK : res;
}
