#ifndef BACKEND_SSL_CONN_REQ_H
#define BACKEND_SSL_CONN_REQ_H

#include "events/io/epoll_manager.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/backend_types.h"

zxy_backend_base_t* zxy_make_ssl_backend_base_conn(void *params);

/////////// ssl connection /////////////

zxy_backend_ssl_conn_t* zxy_make_backend_ssl_conn(SSL_CTX *ctx, int sock_fd);

void zxy_set_up_backend_ssl_base_callbacks(zxy_backend_base_t* backend_base);

#endif /* BACKEND_SSL_CONN_REQ_H */

