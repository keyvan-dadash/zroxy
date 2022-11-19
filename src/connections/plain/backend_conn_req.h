#ifndef BACKEND_CONN_REQ_H
#define BACKEND_CONN_REQ_H

#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/backend_types.h"

/**
 * Create basic backend connection settings.
 * \param params Parameters that should pass for creating backend base.
 * \return zxy_backend_base_t Pointer to the created backend base.
 */
zxy_backend_base_t* zxy_make_plain_backend_base_conn(void *params);

/**
 * Creating backend plain connection.
 * \param sock_fd Socket fd that use in plain connection.
 * \return zxy_backend_conn_t Pointer to the created backend plain connection.
 */
zxy_backend_conn_t* zxy_make_backend_plain_conn(int sock_fd);

/**
 * Set callbacks on backend base.
 * \param backend_base Pointer to the backend base for setting callbacks.
 */
void zxy_set_up_backend_plain_base_callbacks(zxy_backend_base_t* backend_base);

#endif /* BACKEND_CONN_REQ_H */

