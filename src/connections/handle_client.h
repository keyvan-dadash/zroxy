#ifndef HANDLE_CLIENT_H
#define HANDLE_CLIENT_H

/**
 * Handle each new connection.
 * \param client_sock The socket fd of the new client.
 * \param backend_host The network address of the backend which client should connect to. 
 * \param backend_port The port of the backend which client should connect to.
 */
void zxy_handle_client_connection(int client_sock, char *backend_host, char *backend_port);

#endif /* HANDLE_CLIENT_H */

