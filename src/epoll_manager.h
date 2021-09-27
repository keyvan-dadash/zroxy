

#ifndef EPOLL_MANAGER_H
#define EPOLL_MANAGER_H

#include <sys/epoll.h>


#include "defines.h"

extern int epoll_fd;


// typedef struct 
// {
//     char *backend_addr;
//     char *backend_port;
// } connection_params_t;


typedef void (*proxy_callback_func)(void*);

typedef struct
{
    proxy_callback_func on_read;
    proxy_callback_func on_write;
    proxy_callback_func on_close;
} client_handlers_t;


typedef struct
{
    proxy_callback_func on_read;
    proxy_callback_func on_write;
    proxy_callback_func on_close;
} backend_handlers_t;


typedef struct
{
    int client_sock_fd;
    int8_t is_client_closed;
    uint32_t client_events;
    client_handlers_t *client_handlers;

    char *read_buf;
    int32_t max_bufer_size;
    int32_t buffer_ptr;
} client_connection_info_t;

typedef struct
{
    int backend_sock_fd;
    int8_t is_backend_closed;
    uint32_t backend_events;
    backend_handlers_t *backend_handlers;

    char *read_buf;
    int32_t max_bufer_size;
    int32_t buffer_ptr;
} backend_connection_info_t;


typedef struct
{
    client_connection_info_t client_info;
    backend_connection_info_t backend_info;
} proxy_handler_t;


typedef void (*func) (int, uint32_t, void*);

typedef void (*free_params_func) (void*);

typedef struct 
{
    int sock_fd;
    func callback;
    void *params;
    free_params_func free_params;
} handler_t;



void epoll_init();

void add_handler_to_epoll(handler_t *handler, uint32_t mask);

void remove_fd_from_epoll(int sock_fd);

void event_loop();

#endif /* EPOLL_MANAGER_H */