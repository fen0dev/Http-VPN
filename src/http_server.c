#include "http_server.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define PORT 8080
#define MAX_CLIENTS 100

void *thread_function(void *arg);
void log_message(const char *message);

int initialize_http_server() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("setsockopt() protocol succeeded for server_fd\n");   // To check if socket protocol is available

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("HTTP server initialized on port %d\n", PORT);
    return server_fd;
}

SSL_CTX *initialize_ssl() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD *method = SSLv23_server_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

void configure_ssl_context(SSL_CTX *ctx) {
    SSL_CTX_set_ecdh_auto(ctx, 1);
    if (SSL_CTX_use_certificate_file(ctx, "certs/cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, "certs/key.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    read(client_socket, buffer, BUFFER_SIZE);
    printf("Request: %s\n", buffer);
    char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    write(client_socket, response, strlen(response));
    close(client_socket);
}

void handle_ssl_client(SSL *ssl) {
    char buffer[BUFFER_SIZE] = {0};
    SSL_read(ssl, buffer, BUFFER_SIZE);
    printf("Request: %s\n", buffer);
    char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    SSL_write(ssl, response, strlen(response));
    SSL_free(ssl);
}

void *thread_function(void *arg) {
    client_t *client = (client_t *)arg;
    if (client->ssl) {
        handle_ssl_client(client->ssl);
    } else {
        handle_client(client->socket);
    }
    free(client);
    return NULL;
}