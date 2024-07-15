#include "http_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAX_CLIENTS 100

void start_http_server(int http_fd, SSL_CTX *ssl_ctx);

int main(int argc, char *argv[]) {
    int http_fd = initialize_http_server();
    SSL_CTX *ssl_ctx = initialize_ssl();
    configure_ssl_context(ssl_ctx);

    start_http_server(http_fd, ssl_ctx);

    SSL_CTX_free(ssl_ctx);
    return 0;
}

void start_http_server(int http_fd, SSL_CTX *ssl_ctx) {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t threads[MAX_CLIENTS];
    int thread_count = 0;

    while (1) {
        int client_socket;
        if (thread_count >= MAX_CLIENTS) {
            fprintf(stderr, "Maximum clients reached. Waiting for a slot...\n");
            continue;
        }

        client_socket = accept(http_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (client_socket >= 0) {
            SSL *ssl = SSL_new(ssl_ctx);
            SSL_set_fd(ssl, client_socket);
            if (SSL_accept(ssl) <= 0) {
                ERR_print_errors_fp(stderr);
                close(client_socket);
                SSL_free(ssl);
                continue;
            }

            client_t *client = (client_t *)malloc(sizeof(client_t));
            client->socket = client_socket;
            client->ssl = ssl;

            pthread_create(&threads[thread_count++], NULL, thread_function, client);
        }
    }
}