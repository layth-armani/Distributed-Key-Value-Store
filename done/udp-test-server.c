    #include <stdio.h>
    #include <stdlib.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include "socket_layer.h"

    #define CS202_DEFAULT_IP "127.0.0.1"
    #define CS202_DEFAULT_PORT 1234

    int main() {
        int socket_fd = udp_server_init(CS202_DEFAULT_IP, CS202_DEFAULT_PORT, 0);
        if (socket_fd < 0) {
            perror("Error initializing server");
            return EXIT_FAILURE;
        }

        printf("Server listening on %s:%d\n", CS202_DEFAULT_IP, CS202_DEFAULT_PORT);

        while (1) {
            struct sockaddr_in client_addr;
            unsigned int number = 0;

            ssize_t bytes_received = udp_read(socket_fd, (char *)&number, sizeof(number), &client_addr);
            if (bytes_received < 0) {
                perror("Error reading from socket");
                break;
            }

            if (bytes_received != sizeof(number)) {
                fprintf(stderr, "Invalid data size received\n");
                continue;
            }

            number = (number);

            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
            printf("Received message from %s:%d: %u\n", client_ip, ntohs(client_addr.sin_port), number);

            number++;
            number = (number);
            if (udp_send(socket_fd, (char *)&number, sizeof(number), &client_addr) < 0) {
                perror("Error sending response");
            } else {
                printf("Sending message to %s:%d: %u\n", client_ip, ntohs(client_addr.sin_port), (number));
            }
        }

        close(socket_fd);
        return EXIT_SUCCESS;
    }