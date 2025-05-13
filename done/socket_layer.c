/*
 * @file socket_layer.c
 * @brief socket binding for CS-202 project
 *
 */

#include "socket_layer.h"

#include "error.h"
#include "util.h"

#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h> // for struct timeval
#include <unistd.h>

// ======================================================================
int get_socket(time_t t)
{
    int fd = -1;
    if (t < 0) return ERR_INVALID_ARGUMENT;

    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd == -1) return ERR_NETWORK;

    int optval = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0) {
        close(fd);
        return ERR_NETWORK;
    }

    #ifdef SO_REUSEPORT
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) != 0) {
            close(fd);
            return ERR_NETWORK;
        }
    #endif

    if (t > 0) {
        struct timeval timeout;
        zero_init_var(timeout); 

        timeout.tv_sec = t;
        timeout.tv_usec = 0;

        if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != 0) {
            close(fd);
            return ERR_NETWORK;
        }
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

    if(inet_pton(AF_INET, ip, &server_addr.sin_addr)!=1) return ERR_INVALID_ARGUMENT;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if(server_addr.sin_port > MAX_PORT_NUMBER) return ERR_INVALID_ARGUMENT;

    *p_server_addr = server_addr;
    return ERR_NONE;
}

// ======================================================================
int bind_server(int socket, const char *ip, uint16_t port)
{
    struct sockaddr_in server_addr;
    zero_init_var(server_addr);


    int err = get_server_addr(ip, port, &server_addr);
    if(err != ERR_NONE) return err;

    int ret = bind(socket, (const struct sockaddr *) &server_addr, sizeof(server_addr));
    fprintf(stderr, "Bind error : %d \n", errno);

    return ret;
}

// ======================================================================
int udp_server_init(const char *ip, uint16_t port, time_t t)
{
    M_REQUIRE_NON_NULL(ip);

    // Create a socket
    const int socket = get_socket(t);
    if (socket < 0) {
        perror("Error opening socket");
        return ERR_NETWORK;
    }

    int err = bind_server(socket, ip, port);
    if (err != ERR_NONE) {
        close(socket);
        perror("Error binding socket");
        return err;
    }

    return socket;
}

/********************************************************************/
ssize_t udp_read(int socket, char *buf, size_t buflen,
                 struct sockaddr_in *cli_addr)
{
    M_REQUIRE_NON_NULL(buf);

    socklen_t addr_len = sizeof(struct sockaddr_in);
    ssize_t res = recvfrom(socket, buf, buflen, 0, 
        cli_addr ? (struct sockaddr *)cli_addr : NULL, cli_addr ? &addr_len : NULL);

    return res < 0 ? ERR_NETWORK : res;
}

/********************************************************************/
ssize_t udp_send(int socket, const char *response, size_t response_len,
                 const struct sockaddr_in *cli_addr)
{
    M_REQUIRE_NON_NULL(response);

    socklen_t addr_len = sizeof(struct sockaddr_in);
    const ssize_t res = sendto(socket, response, response_len, 0, 
        cli_addr ? (struct sockaddr *)cli_addr : NULL, addr_len);

    return res < 0 ? ERR_NETWORK : res;
}
