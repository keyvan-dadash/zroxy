


#ifndef PROXY_TYPES_H
#define PROXY_TYPES_H

#include <sys/types.h>

#include "connections/conntypes/client_types.h"
#include "connections/conntypes/backend_types.h"

typedef void (*proxy_callback_func)(void*);


enum zxy_conn_type
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

    /**
     * refrence counter
     */
    int refrence_counter;
} zxy_proxy_connection_t;


#endif /* PROXY_TYPES_H */