

#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include "events/io/epoll_manager.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/client_types.h"
#include "connections/conntypes/backend_types.h"
#include "connections/conntypes/proxy_types.h"

zxy_event_handler_t* make_proxy_event_handler(zxy_proxy_connection_t*, int, int8_t);

zxy_proxy_connection_t* zxy_make_proxy_connection(int32_t,
            enum zxy_conn_type, 
            int32_t,
            enum zxy_conn_type);

zxy_client_base_t* zxy_make_client_conn_with_type(int32_t, enum zxy_conn_type);

zxy_backend_base_t* zxy_make_backend_conn_with_type(int32_t, enum zxy_conn_type);

#endif /* CONNECTIONS_H */