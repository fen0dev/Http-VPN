#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <openssl/ssl.h>
#include <openssl/err.h>

typedef struct {
    int socket;
    SSL *ssl;
} client_t;

int initialize_http_server();
SSL_CTX *initialize_ssl();
void configure_ssl_context(SSL_CTX *ctx);
void handle_client(int client_socket);
void handle_ssl_client(SSL *ssl);
void *thread_function(void *arg);
void log_message(const char *message);

#endif