#include "node.h"
#include "ring.h"
#include "config.h"

#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>

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
void print_sha(unsigned char* sha){
    for (size_t i = 0; i < SHA_DIGEST_LENGTH; ++i)
        {
            printf("%02x", sha[i]);
        }
}

// ======================================================================
int main(void)
{
    ring_t servers = {0, 0, NULL};
    int err = ring_init(&servers);
    if (err != ERR_NONE) return err;

    printf("Ring nodes:\n");
    for (size_t i = 0; i < servers.size; i++)
    {
       print_sha(servers.nodes[i].sha);
       printf(" (%s %d)\n", inet_ntoa(servers.nodes[i].addr_s.sin_addr),
           ntohs(servers.nodes[i].addr_s.sin_port)
          );
    }

    putchar('\n');
    
    int socket = get_socket(1);
    if(socket < 0) {
        ring_free(&servers);
        return socket;
    }

    Htable_t* table = Htable_construct(servers.size);
    if(table == NULL){
        ring_free(&servers);
        close(socket);
        return ERR_OUT_OF_MEMORY;
    }
    
    
    for (size_t i = 0; i < servers.size; i++) {
        char key[INET_ADDRSTRLEN + 8];
        memset(key, 0, INET_ADDRSTRLEN + 8);
        char* value;
        snprintf(key, sizeof(key), "%s:%d", inet_ntoa(servers.nodes[i].addr_s.sin_addr), ntohs(servers.nodes[i].addr_s.sin_port));
        value = Htable_get_value(table, key);

        if (value == NULL)
        {
            unsigned char idx = (unsigned char)i;
            char str[2] = { (char)idx, '\0' };
            err = Htable_add_value(table, key, str);
            if(err != ERR_NONE){
                ring_free(&servers);
                close(socket);
                Htable_free(&table);
                return err;
            }
        }
        else {
            print_server(NULL, &servers.nodes[i], ": ");
            printf("node %zu has same server as node %d\n",i, (int)value[0]);
            putchar('\n');
            free(value);
            continue;
        }
        

        ssize_t bytes = udp_send(socket, "", 0, &servers.nodes[i].addr_s);
        if(bytes < 0) {
            printf("Error when trying to send to ");
            print_server("", &servers.nodes[i], "\n");
            ring_free(&servers);
            close(socket);
            Htable_free(&table);
            free(value);
            return (int)bytes;
        }

        print_server("", &servers.nodes[i], ":\n");
        
        char buffer[MAX_MSG_SIZE];
        struct sockaddr_in cli_addr;
            
        while (1) {
            
            memset(buffer, 0, sizeof(buffer));
            memset(&cli_addr, 0, sizeof(cli_addr));
            errno = 0;
            bytes = udp_read(socket, buffer, MAX_MSG_SIZE, &cli_addr);

            if(bytes < 0){
                if(errno == EAGAIN)break;
                printf("Error when trying to receive from ");
                print_server("", &servers.nodes[i], "\n");
                ring_free(&servers);
                close(socket);
                Htable_free(&table);
                free(value);
                return (int)bytes;
            }

            if (cli_addr.sin_addr.s_addr != servers.nodes[i].addr_s.sin_addr.s_addr ||
                cli_addr.sin_port != servers.nodes[i].addr_s.sin_port) {
                printf("/!\\ FROM "); print_sockaddr_in(&cli_addr); printf(":\n");
                printf("%.*s", (int)bytes, buffer);
            }
            else printf("%.*s", (int)bytes, buffer);
            

            printf("\n");
        }
        free(value);
    }

    ring_free(&servers);
    close(socket);
    Htable_free(&table);
    

    return ERR_NONE;
}
