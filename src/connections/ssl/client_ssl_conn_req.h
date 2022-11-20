/**
 * @file client_ssl_conn_req.h
 */

#ifndef CLIENT_SSL_CONN_REQ_H
#define CLIENT_SSL_CONN_REQ_H

#include "events/io/epoll_manager.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/client_types.h"

/**
 * Create basic ssl client connection settings.
 * \param params Parameters that should pass for creating client base.
 * \return zxy_client_base_t Pointer to the created client base.
 */
zxy_client_base_t* zxy_make_ssl_client_base_conn(void *params);

/**
 * Creating client ssl connection.
 * \param ctx The SSL_CTX that use in ssl connection procedure.
 * \param sock_fd Socket fd that use in ssl connection.
 * \return zxy_client_ssl_conn_t Pointer to the created client ssl connection.
 */
zxy_client_ssl_conn_t* zxy_make_client_ssl_conn(SSL_CTX *ctx, int sock_fd);

/**
 * Set callbacks on client base.
 * \param client_base Pointer to the client base for setting callbacks.
 */
void zxy_set_up_client_ssl_base_callbacks(zxy_client_base_t* client_base);

#endif /* CLIENT_SSL_CONN_REQ_H */

