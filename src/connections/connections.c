



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "logging/logs.h"
#include "connections/connections.h"
#include "connections/plain/backend_conn_req.h"
#include "connections/plain/client_conn_req.h"
#include "connections/conntypes/proxy_types.h"
#include "events/peers/proxy_callbacks.h"


zxy_event_handler_t* make_proxy_event_handler(zxy_proxy_connection_t* proxy_obj, int sock, int8_t sock_type)
{
    zxy_event_handler_t* event_handler = (zxy_event_handler_t*)malloc(sizeof(zxy_event_handler_t));

    //add refrence 
    proxy_obj->refrence_counter++;

    event_handler->sock_fd = sock;
    event_handler->params = (void*)proxy_obj;
    event_handler->free_params = zxy_free_proxy_obj;
    event_handler->sock_type = sock_type;
    event_handler->callback = zxy_proxy_events_callback;
    
    return event_handler;
}

zxy_proxy_connection_t* zxy_make_proxy_connection(
            int32_t client_fd,
            enum zxy_conn_type client_conn_type, 
            int32_t backend_fd,
            enum zxy_conn_type backend_conn_type)
{
    zxy_proxy_connection_t *proxy_conn = (zxy_proxy_connection_t*)calloc(1, sizeof(zxy_proxy_connection_t));

    proxy_conn->client = zxy_make_client_conn_with_type(client_fd, client_conn_type);
    proxy_conn->backend = zxy_make_backend_conn_with_type(backend_fd, backend_conn_type);
    proxy_conn->refrence_counter = 0;

    return proxy_conn;
}

zxy_client_base_t* zxy_make_client_conn_with_type(int32_t client_fd, enum zxy_conn_type client_conn_type)
{
    zxy_client_base_t *client_base;
    switch (client_conn_type)
    {
    case PLAIN_CONN: {
        zxy_client_conn_t *client_plain_conn = zxy_make_client_plain_conn(client_fd);
        client_base = zxy_make_plain_client_base_conn((void*)client_plain_conn);
        zxy_set_up_client_plain_base_callbacks(client_base);
        break;
    }

    case SSL_CONN: {
        //TODO: handle ssl
        break;
    }
    default:
        LOG_FATAL(-1, "Client connection type unkown: %d", client_conn_type);
    }

    return client_base;
}

zxy_backend_base_t* zxy_make_backend_conn_with_type(int32_t backend_fd, enum zxy_conn_type backend_conn_type)
{
    zxy_backend_base_t *backend_base;
    switch (backend_conn_type)
    {
    case PLAIN_CONN: {
        zxy_backend_conn_t *backend_plain_conn = zxy_make_backend_plain_conn(backend_fd);
        backend_base = zxy_make_plain_backend_base_conn((void*)backend_plain_conn);
        zxy_set_up_backend_plain_base_callbacks(backend_base);
        break;
    }

    case SSL_CONN: {
        //TODO: handle ssl
        break;
    }

    default:
        LOG_FATAL(-1, "Backend connection type unkown: %d", backend_conn_type);
    }

    return backend_base;
}