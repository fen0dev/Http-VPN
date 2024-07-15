#ifndef VPN_SERVER_H
#define VPN_SERVER_H

void *vpn_thread_function(void *arg);
int initialize_vpn_server();
void handle_vpn_client(int client_socket);
void log_message(const char *message);  // Declare log_message

#endif