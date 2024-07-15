#include "vpn_server.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define VPN_PORT 1194
#define MAX_VPN_CLIENTS 100

void *vpn_thread_function(void *arg);
void log_message(const char *message);


int initialize_vpn_server() {
    int vpn_fd;
    struct sockaddr_in vpn_address;
    int opt = 1;

    // Creating VPN socket file descriptor
    if ((vpn_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("VPN socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching VPN socket to the port 1194
    if (setsockopt(vpn_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("VPN setsockopt");
        close(vpn_fd);
        exit(EXIT_FAILURE);
    }
    printf("setsockopt() protocol succeeded for vpn_fd\n");

    vpn_address.sin_family = AF_INET;
    vpn_address.sin_addr.s_addr = INADDR_ANY;
    vpn_address.sin_port = htons(VPN_PORT);

    // Forcefully attaching VPN socket to the port 1194
    if (bind(vpn_fd, (struct sockaddr *)&vpn_address, sizeof(vpn_address)) < 0) {
        perror("VPN bind failed");
        close(vpn_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(vpn_fd, MAX_VPN_CLIENTS) < 0) {
        perror("VPN listen");
        close(vpn_fd);
        exit(EXIT_FAILURE);
    }

    printf("[+] VPN initialized on port %d\n", VPN_PORT);
    return vpn_fd;
}

void handle_vpn_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    read(client_socket, buffer, BUFFER_SIZE);
    printf("VPN Data: %s\n", buffer);  // Correct the typo
    write(client_socket, buffer, strlen(buffer));  // Echo back the data
    close(client_socket);
}

void *vpn_thread_function(void *arg) {
    int client_socket = *((int *)arg);
    free(arg);
    handle_vpn_client(client_socket);
    return NULL;
}