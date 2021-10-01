



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#include "timers.h"
#include "timer_callback.h"
#include "connections.h"
#include "epoll_manager.h"
#include "client_callbacks.h"
#include "backend_callbacks.h"

#include "connections/conntypes/proxy_types.h"

void free_proxy_handler(zxy_proxy_connection_t *proxy_obj)
{
    free(proxy_obj);
}

void free_client_requirments(void *ptr)
{
    zxy_backend_base_t *backend = &( ((zxy_proxy_connection_t*)ptr)->backend_info );
    zxy_client_base_t *client = &( ((zxy_proxy_connection_t*)ptr)->client_info );
    free(client->client_handlers);

    if (backend->set_free == 1) {
        free(backend->read_buf);
    }

    if (client->is_client_closed && backend->is_backend_closed) {
        free(client->read_buf);
        client->set_free = -1;
    } else {
        client->set_free = 1;
    }

    printf("(free client) client set free is %d and back is %d\n", client->set_free, backend->set_free);

    if ( (backend->set_free != 0) ) {
        free_proxy_handler((zxy_proxy_connection_t*)ptr);
    }
}

void free_backend_requirments(void *ptr)
{
    zxy_backend_base_t *backend = &( ((zxy_proxy_connection_t*)ptr)->backend_info );
    zxy_client_base_t *client = &( ((zxy_proxy_connection_t*)ptr)->client_info );
    free(backend->backend_handlers);
    
    if (client->set_free == 1) {
        free(client->read_buf);
    }

    if (client->is_client_closed && backend->is_backend_closed) {
        free(backend->read_buf);
        backend->set_free = -1;
    } else {
        backend->set_free = 1;
    }

    printf("(free back) client set free is %d and back is %d\n", client->set_free, backend->set_free);

    if ( (client->set_free != 0)  ) {
        free_proxy_handler((zxy_proxy_connection_t*)ptr);
    }
}

handler_t* make_client_handler(zxy_proxy_connection_t *proxy_handler, int client_sock_fd)
{
    zxy_client_base_t client_conn_info;
    client_conn_info.client_sock_fd = client_sock_fd;
    client_conn_info.is_client_closed = 0;
    client_conn_info.max_bufer_size = READ_BUF_SIZE + 1;
    client_conn_info.read_buf = (char *)malloc(sizeof(char) * client_conn_info.max_bufer_size);
    client_conn_info.buffer_ptr = 0;
    client_conn_info.client_events = 0;
    client_conn_info.set_free = 0;

    proxy_handler->client_info = client_conn_info;

    proxy_handler->client_info.client_handlers = (client_handlers_t *)malloc(sizeof(client_handlers_t));
    proxy_handler->client_info.client_handlers->on_read = on_client_read_event;
    proxy_handler->client_info.client_handlers->on_write = on_client_write_event;
    proxy_handler->client_info.client_handlers->on_close = on_client_close_event;

    handler_t *handler = (handler_t *)malloc(sizeof(handler_t));
    handler->sock_fd = client_sock_fd;
    handler->params = proxy_handler;
    handler->free_params = free_client_requirments;
    handler->callback = client_on_event_callback;

    return handler;
}

handler_t* make_backend_handler(zxy_proxy_connection_t *proxy_handler, int backend_sock_fd)
{
    zxy_backend_base_t backend_conn_info;
    backend_conn_info.backend_sock_fd = backend_sock_fd;
    backend_conn_info.is_backend_closed = 0;
    backend_conn_info.max_bufer_size = READ_BUF_SIZE + 1;
    backend_conn_info.read_buf = (char *)malloc(sizeof(char) * backend_conn_info.max_bufer_size);
    backend_conn_info.buffer_ptr = 0;
    backend_conn_info.backend_events = 0;
    backend_conn_info.set_free = 0;

    proxy_handler->backend_info = backend_conn_info;

    proxy_handler->backend_info.backend_handlers = (backend_handlers_t *)malloc(sizeof(backend_handlers_t));
    proxy_handler->backend_info.backend_handlers->on_read = on_backend_read_event;
    proxy_handler->backend_info.backend_handlers->on_write = on_backend_write_event;
    proxy_handler->backend_info.backend_handlers->on_close = on_backend_close_event;

    handler_t *handler = (handler_t *)malloc(sizeof(handler_t));
    handler->sock_fd = backend_sock_fd;
    handler->params = proxy_handler;
    handler->free_params = free_backend_requirments;
    handler->callback = backend_on_event_callback;

    return handler;
}

void make_proxy_connection(int client_sock_fd, int backend_sock_fd)
{
    zxy_proxy_connection_t *proxy_obj = (zxy_proxy_connection_t *)malloc(sizeof(zxy_proxy_connection_t));

    handler_t *client_handler = make_client_handler(proxy_obj, client_sock_fd);
    handler_t *backend_handler = make_backend_handler(proxy_obj, backend_sock_fd);

    int32_t tfd = create_timer_with_expiration(1, 0);
    handler_t *timer_handler = make_timer_handler(tfd, proxy_obj);

    proxy_obj->client_handler_ptr = client_handler;
    proxy_obj->backend_handler_ptr = backend_handler;
    proxy_obj->timer_handler_ptr = timer_handler;
    proxy_obj->timer_status_and_fd = tfd;


    add_handler_to_epoll(client_handler, EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLET);
    add_handler_to_epoll(backend_handler, EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLET);
    add_handler_to_epoll(timer_handler, EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLET);
}