#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "socket_layer.h"
#include "error.h"

#define CS202_DEFAULT_IP "127.0.0.1"
#define CS202_DEFAULT_PORT 1234

int main(void) {
    int socket_fd = get_socket(5);
    if (socket_fd < 0) {
        perror("Error creating socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    if (get_server_addr(CS202_DEFAULT_IP, CS202_DEFAULT_PORT, &server_addr) != ERR_NONE) {
        perror("Error setting server address");
        close(socket_fd);
        return EXIT_FAILURE;
    }

    printf("What int value do you want to send? ");
    unsigned int number;
    if (scanf("%u", &number) != 1) {
        fprintf(stderr, "Invalid input\n");
        close(socket_fd);
        return EXIT_FAILURE;
    }

    printf("Sending message to %s:%d: %u\n", CS202_DEFAULT_IP, CS202_DEFAULT_PORT, number);
    if (udp_send(socket_fd, (char *)&number, sizeof(number), &server_addr) < 0) {
        perror("Error sending data");
        close(socket_fd);
        return EXIT_FAILURE;
    }

    unsigned int response;
    ssize_t bytes_received = udp_read(socket_fd, (char *)&response, sizeof(response), NULL);
    if (bytes_received < 0) {
        perror("Error receiving response");
        close(socket_fd);
        return EXIT_FAILURE;
    }

    printf("Received response: %u\n", response);

    close(socket_fd);
    return EXIT_SUCCESS;
}