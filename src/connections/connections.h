

#ifndef CONNECTIONS_H
#define CONNECTIONS_H


#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/client_types.h"
#include "connections/conntypes/backend_types.h"
#include "connections/conntypes/proxy_types.h"

zxy_proxy_connection_t* zxy_make_proxy_connection(int32_t client_fd,
            enum zxy_client_conn_type* client_base, 
            int32_t backend_fd,
            enum zxy_backend_conn_type* backend_base);

zxy_client_base_t* zxy_make_client_conn_with_type(int32_t client_fd, enum zxy_client_conn_type);

zxy_backend_base_t* zxy_make_backend_conn_with_type(int32_t backend_fd, enum zxy_backend_conn_type);

#endif /* CONNECTIONS_H */