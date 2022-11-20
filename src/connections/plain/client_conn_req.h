/**
 * @file client_conn_req.h
 */

#ifndef CLIENT_CONN_REQ_H
#define CLIENT_CONN_REQ_H

#include "events/io/epoll_manager.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/client_types.h"

/**
 * Create basic client connection settings.
 * \param params Parameters that should pass for creating client base.
 * \return zxy_client_base_t Pointer to the created client base.
 */
zxy_client_base_t* zxy_make_plain_client_base_conn(void *params);

/**
 * Creating client plain connection.
 * \param sock_fd Socket fd that use in plain connection.
 * \return zxy_client_conn_t Pointer to the created client plain connection.
 */
zxy_client_conn_t* zxy_make_client_plain_conn(int sock_fd);

/**
 * Set callbacks on client base.
 * \param client_base Pointer to the client base for setting callbacks.
 */
void zxy_set_up_client_plain_base_callbacks(zxy_client_base_t* client_base);

#endif /* CLIENT_CONN_REQ_H */

