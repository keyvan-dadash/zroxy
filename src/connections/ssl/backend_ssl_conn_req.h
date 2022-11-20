/**
 * @file backend_ssl_conn_req.h
 */

#ifndef BACKEND_SSL_CONN_REQ_H
#define BACKEND_SSL_CONN_REQ_H

#include "events/io/epoll_manager.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/backend_types.h"

/**
 * Create basic ssl backend connection settings.
 * \param params Parameters that should pass for creating backend base.
 * \return zxy_backend_base_t Pointer to the created backend base.
 */
zxy_backend_base_t* zxy_make_ssl_backend_base_conn(void *params);

/**
 * Creating backend ssl connection.
 * \param ctx The SSL_CTX that use in ssl connection procedure.
 * \param sock_fd Socket fd that use in ssl connection.
 * \return zxy_backend_ssl_conn_t Pointer to the created backend ssl connection.
 */
zxy_backend_ssl_conn_t* zxy_make_backend_ssl_conn(SSL_CTX *ctx, int sock_fd);

/**
 * Set callbacks on backend base.
 * \param backend_base Pointer to the backend base for setting callbacks.
 */
void zxy_set_up_backend_ssl_base_callbacks(zxy_backend_base_t* backend_base);

#endif /* BACKEND_SSL_CONN_REQ_H */

