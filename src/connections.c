



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#include "connections.h"
#include "epoll_manager.h"
#include "client_callbacks.h"
#include "backend_callbacks.h"


//TODO: add call back for each client and backend sepratly in handler

void free_proxy_handler(proxy_handler_t *proxy_obj)
{
    free(proxy_obj);
}

void free_client_requirments(void *ptr)
{
    client_connection_info_t client = ((proxy_handler_t*)ptr)->client_info;
    backend_connection_info_t backend = ((proxy_handler_t*)ptr)->backend_info;
    free(client.read_buf);
    if (client.is_client_closed && backend.is_backend_closed)
        free_proxy_handler((proxy_handler_t*)ptr);
}

void free_backend_requirments(void *ptr)
{
    backend_connection_info_t backend = ((proxy_handler_t*)ptr)->backend_info;
    client_connection_info_t client = ((proxy_handler_t*)ptr)->client_info;
    free(backend.read_buf);
    if (client.is_client_closed && backend.is_backend_closed)
        free_proxy_handler((proxy_handler_t*)ptr);
}

handler_t* make_client_handler(proxy_handler_t *proxy_handler, int client_sock_fd)
{
    client_connection_info_t client_conn_info;
    client_conn_info.client_sock_fd = client_sock_fd;
    client_conn_info.is_client_closed = 0;
    client_conn_info.max_bufer_size = READ_BUF_SIZE;
    client_conn_info.read_buf = malloc(sizeof(char) * READ_BUF_SIZE);

    proxy_handler->client_info = client_conn_info;
    proxy_handler->client_info.client_handlers->on_read = on_client_read_event;
    proxy_handler->client_info.client_handlers->on_write = on_client_write_event;
    proxy_handler->client_info.client_handlers->on_close = on_client_close_event;

    handler_t *handler = malloc(sizeof(handler_t));
    handler->sock_fd = client_sock_fd;
    handler->params = proxy_handler;
    handler->free_params = free_client_requirments;
    handler->callback = client_on_event_callback;

    return handler;
}

handler_t* make_backend_handler(proxy_handler_t *proxy_handler, int backend_sock_fd)
{
    backend_connection_info_t backend_conn_info;
    backend_conn_info.backend_sock_fd = backend_sock_fd;
    backend_conn_info.is_backend_closed = 0;
    backend_conn_info.max_bufer_size = READ_BUF_SIZE;
    backend_conn_info.read_buf = malloc(sizeof(char) * READ_BUF_SIZE);

    proxy_handler->backend_info = backend_conn_info;
    proxy_handler->backend_info.backend_handlers->on_read = on_backend_read_event;
    proxy_handler->backend_info.backend_handlers->on_write = on_backend_write_event;
    proxy_handler->backend_info.backend_handlers->on_close = on_backend_close_event;

    handler_t *handler = malloc(sizeof(handler_t));
    handler->sock_fd = backend_sock_fd;
    handler->params = proxy_handler;
    handler->free_params = free_backend_requirments;
    handler->callback = backend_on_event_callback;

    return handler;
}

void make_proxy_connection(int client_sock_fd, int backend_sock_fd)
{
    proxy_handler_t *proxy_obj = malloc(sizeof(proxy_handler_t));

    handler_t *client_handler = make_client_handler(proxy_obj, client_sock_fd);
    handler_t *backend_handler = make_client_handler(proxy_obj, backend_sock_fd);


    add_handler_to_epoll(client_handler, EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR);
    add_handler_to_epoll(backend_handler, EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR);
}