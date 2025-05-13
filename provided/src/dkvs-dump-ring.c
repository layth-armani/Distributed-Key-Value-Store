#include "node.h"

#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// ======================================================================
static void print_sockaddr_in(const struct sockaddr_in* addr_s)
{
    printf("%s:%d", inet_ntoa(addr_s->sin_addr),
           ntohs(addr_s->sin_port)
          );
}

// ======================================================================
static void print_server(const char*   prefix,
                         const node_t* node,
                         const char*   suffix
                        )
{
    if (prefix) fputs(prefix, stdout);
    print_sockaddr_in(&node->addr_s);
    if (suffix) fputs(suffix, stdout);
}

// ======================================================================
int main(void)
{
    return 0;
}
