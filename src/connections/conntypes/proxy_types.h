


#ifndef PROXY_TYPES_H
#define PROXY_TYPES_H

#include <sys/types.h>

#include "connections/conntypes/client_types.h"
#include "connections/conntypes/backend_types.h"

typedef void (*proxy_callback_func)(void*);

typedef struct
{
    proxy_callback_func on_read;
    proxy_callback_func on_write;
    proxy_callback_func on_close;
} zxy_callbacks_t;

typedef struct
{
    int sock_fd;
    int8_t is_closed;
    u_int32_t events;
    zxy_callbacks_t *callback_funcs;

    char *read_buffer;
    int32_t max_bufer_size;
    int32_t buffer_location_ptr;
    int8_t set_free;
} zxy_connection_info_t;



enum zxy_client_conn_type
{
    PLAIN_CONN,
    SSL_CONN
};

enum zxy_backend_conn_type
{
    PLAIN_CONN,
    SSL_CONN
};


typedef struct 
{
    zxy_client_base_t *client;
    zxy_backend_base_t *backend;

    void *client_handler_ptr; //workaround: cannot free memory of handler when calling epoll_ctl with remove flag
    void *backend_handler_ptr; //workaround: cannot free memory of handler when calling epoll_ctl with remove flag
    void *timer_handler_ptr; //workaround: cannot free memory of handler when calling epoll_ctl with remove flag

    int32_t timer_status_and_fd;
} zxy_proxy_connection_t;



// typedef struct
// {
//     int client_sock_fd;
//     int8_t is_client_closed;
//     uint32_t client_events;
//     client_handlers_t *client_handlers;

//     char *read_buf;
//     int32_t max_bufer_size;
//     int32_t buffer_ptr;
//     int8_t set_free;
// } client_connection_info_t;

// typedef struct
// {
//     int backend_sock_fd;
//     int8_t is_backend_closed;
//     uint32_t backend_events;
//     backend_handlers_t *backend_handlers;

//     char *read_buf;
//     int32_t max_bufer_size;
//     int32_t buffer_ptr;
//     int8_t set_free;
// } backend_connection_info_t;


// typedef struct
// {
//     client_connection_info_t client_info;
//     backend_connection_info_t backend_info;

//     void *client_handler_ptr; //workaround: cannot free memory of handler when calling epoll_ctl with remove flag
//     void *backend_handler_ptr; //workaround: cannot free memory of handler when calling epoll_ctl with remove flag
//     void *timer_handler_ptr; //workaround: cannot free memory of handler when calling epoll_ctl with remove flag

//     int32_t timer_status_and_fd;
// } proxy_handler_t;



#endif /* PROXY_TYPES_H */