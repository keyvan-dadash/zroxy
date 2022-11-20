/**
 * @file proxy_types.h
 */
#ifndef PROXY_TYPES_H
#define PROXY_TYPES_H

#include <sys/types.h>

#include "connections/conntypes/client_types.h"
#include "connections/conntypes/backend_types.h"

typedef void (*proxy_callback_func)(void*);

/**
 * The enumeration of connection types.
 */
enum zxy_conn_type
{
    /**
     * Connection type is plain.
     */
    PLAIN_CONN,

    /**
     * Connection type is ssl.
     */
    SSL_CONN
};

/**
 * Represent each connection settings and config in zroxy.
 */
typedef struct 
{
    /**
     * Client connection parameters and settings.
     */
    zxy_client_base_t *client;

    /**
     * Backend connection parameters and settings.
     */
    zxy_backend_base_t *backend;

    /**
     * Pointer to handler function for handling client connection events.
     */
    void *client_handler_ptr;

    /**
     * Pointer to handler function for handling backend connection events.
     */
    void *backend_handler_ptr;

    /**
     * Pointer to handler function for handling timeout event.
     */
    void *timer_handler_ptr;

    /**
     * Shows timer current status and socket fd.
     */
    int32_t timer_status_and_fd;

    /**
     * Refrence counter for determining when should zroxy free allocated proxy connection settings.
     */
    int refrence_counter;
} zxy_proxy_connection_t;

#endif /* PROXY_TYPES_H */

