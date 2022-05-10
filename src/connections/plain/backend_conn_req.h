#ifndef BACKEND_CONN_REQ_H
#define BACKEND_CONN_REQ_H

#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/backend_types.h"

zxy_backend_base_t* zxy_make_plain_backend_base_conn(void *params);

//////// plain connections
zxy_backend_conn_t* zxy_make_backend_plain_conn(int sock_fd);

void zxy_set_up_backend_plain_base_callbacks(zxy_backend_base_t* backend_base);

#endif /* BACKEND_CONN_REQ_H */

