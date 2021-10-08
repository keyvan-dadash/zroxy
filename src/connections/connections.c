



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


// #include "timers.h"
// #include "timer_callback.h"
// #include "epoll_manager.h"
// #include "client_callbacks.h"
// #include "backend_callbacks.h"

#include "logging/logs.h"
#include "connections/connections.h"
#include "connections/backend_conn_req.h"
#include "connections/client_conn_req.h"
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
        client_base = zxy_make_client_base_conn((void*)client_plain_conn);
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
        backend_base = zxy_make_backend_base_conn((void*)backend_plain_conn);
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



// void free_proxy_handler(zxy_proxy_connection_t *proxy_obj)
// {
//     free(proxy_obj);
// }

// void free_client_requirments(void *ptr)
// {
//     zxy_backend_base_t *backend = &( ((zxy_proxy_connection_t*)ptr)->backend_info );
//     zxy_client_base_t *client = &( ((zxy_proxy_connection_t*)ptr)->client_info );
//     free(client->client_handlers);

//     if (backend->set_free == 1) {
//         free(backend->read_buf);
//     }

//     if (client->is_client_closed && backend->is_backend_closed) {
//         free(client->read_buf);
//         client->set_free = -1;
//     } else {
//         client->set_free = 1;
//     }

//     printf("(free client) client set free is %d and back is %d\n", client->set_free, backend->set_free);

//     if ( (backend->set_free != 0) ) {
//         free_proxy_handler((zxy_proxy_connection_t*)ptr);
//     }
// }

// void free_backend_requirments(void *ptr)
// {
//     zxy_backend_base_t *backend = &( ((zxy_proxy_connection_t*)ptr)->backend_info );
//     zxy_client_base_t *client = &( ((zxy_proxy_connection_t*)ptr)->client_info );
//     free(backend->backend_handlers);
    
//     if (client->set_free == 1) {
//         free(client->read_buf);
//     }

//     if (client->is_client_closed && backend->is_backend_closed) {
//         free(backend->read_buf);
//         backend->set_free = -1;
//     } else {
//         backend->set_free = 1;
//     }

//     printf("(free back) client set free is %d and back is %d\n", client->set_free, backend->set_free);

//     if ( (client->set_free != 0)  ) {
//         free_proxy_handler((zxy_proxy_connection_t*)ptr);
//     }
// }

// handler_t* make_client_handler(zxy_proxy_connection_t *proxy_handler, int client_sock_fd)
// {
//     zxy_client_base_t client_conn_info;
//     client_conn_info.client_sock_fd = client_sock_fd;
//     client_conn_info.is_client_closed = 0;
//     client_conn_info.max_bufer_size = READ_BUF_SIZE + 1;
//     client_conn_info.read_buf = (char *)malloc(sizeof(char) * client_conn_info.max_bufer_size);
//     client_conn_info.buffer_ptr = 0;
//     client_conn_info.client_events = 0;
//     client_conn_info.set_free = 0;

//     proxy_handler->client_info = client_conn_info;

//     proxy_handler->client_info.client_handlers = (client_handlers_t *)malloc(sizeof(client_handlers_t));
//     proxy_handler->client_info.client_handlers->on_read = on_client_read_event;
//     proxy_handler->client_info.client_handlers->on_write = on_client_write_event;
//     proxy_handler->client_info.client_handlers->on_close = on_client_close_event;

//     handler_t *handler = (handler_t *)malloc(sizeof(handler_t));
//     handler->sock_fd = client_sock_fd;
//     handler->params = proxy_handler;
//     handler->free_params = free_client_requirments;
//     handler->callback = client_on_event_callback;

//     return handler;
// }

// handler_t* make_backend_handler(zxy_proxy_connection_t *proxy_handler, int backend_sock_fd)
// {
//     zxy_backend_base_t backend_conn_info;
//     backend_conn_info.backend_sock_fd = backend_sock_fd;
//     backend_conn_info.is_backend_closed = 0;
//     backend_conn_info.max_bufer_size = READ_BUF_SIZE + 1;
//     backend_conn_info.read_buf = (char *)malloc(sizeof(char) * backend_conn_info.max_bufer_size);
//     backend_conn_info.buffer_ptr = 0;
//     backend_conn_info.backend_events = 0;
//     backend_conn_info.set_free = 0;

//     proxy_handler->backend_info = backend_conn_info;

//     proxy_handler->backend_info.backend_handlers = (backend_handlers_t *)malloc(sizeof(backend_handlers_t));
//     proxy_handler->backend_info.backend_handlers->on_read = on_backend_read_event;
//     proxy_handler->backend_info.backend_handlers->on_write = on_backend_write_event;
//     proxy_handler->backend_info.backend_handlers->on_close = on_backend_close_event;

//     handler_t *handler = (handler_t *)malloc(sizeof(handler_t));
//     handler->sock_fd = backend_sock_fd;
//     handler->params = proxy_handler;
//     handler->free_params = free_backend_requirments;
//     handler->callback = backend_on_event_callback;

//     return handler;
// }

// void make_proxy_connection(int client_sock_fd, int backend_sock_fd)
// {
//     zxy_proxy_connection_t *proxy_obj = (zxy_proxy_connection_t *)malloc(sizeof(zxy_proxy_connection_t));

//     handler_t *client_handler = make_client_handler(proxy_obj, client_sock_fd);
//     handler_t *backend_handler = make_backend_handler(proxy_obj, backend_sock_fd);

//     int32_t tfd = create_timer_with_expiration(1, 0);
//     handler_t *timer_handler = zxy_make_timer_handler(tfd, proxy_obj);

//     proxy_obj->client_handler_ptr = client_handler;
//     proxy_obj->backend_handler_ptr = backend_handler;
//     proxy_obj->timer_handler_ptr = timer_handler;
//     proxy_obj->timer_status_and_fd = tfd;


//     add_handler_to_epoll(client_handler, EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLET);
//     add_handler_to_epoll(backend_handler, EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLET);
//     add_handler_to_epoll(timer_handler, EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLET);
// }