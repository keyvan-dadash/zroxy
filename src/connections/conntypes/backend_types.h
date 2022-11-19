#ifndef BACKEND_TYPES_H
#define BACKEND_TYPES_H

#include <sys/types.h>

#include <openssl/ssl.h>
#include <openssl/bio.h>

#include "utils/io/buffer_manager.h"
#include "utils/io/io_helper.h"

typedef int (*zxy_backend_read_callback_func)(void*);

typedef int (*zxy_backend_write_callback_func)(void*, zxy_write_io_req_t*);

typedef int (*zxy_backend_close_callback_func)(void*);

typedef zxy_write_io_req_t (*zxy_backend_request_buffer_reader)(void*);

typedef int (*zxy_backend_read_nbytes_from_buffer_func)(void*, int32_t);

typedef void (*zxy_backend_event_callback)(int32_t, u_int32_t, void*);

typedef int  (*zxy_backend_memebr_func)(u_int32_t, u_int32_t, void*);

typedef void (*zxy_backend_free_params)(void*);

/**
 * Backend base connection which is all other type of backend connection implementing this.
 */
typedef struct 
{
    /**
     * Read callback.
     */
    zxy_backend_read_callback_func on_read;

    /**
     * Write callback.
     */
    zxy_backend_write_callback_func on_write;

    /**
     * Close callback.
     */
    zxy_backend_close_callback_func on_close;

    /**
     * Request buffer for read operation.
     */
    zxy_backend_request_buffer_reader request_buffer_reader;

    /**
     * Function for advance pointer of underlying buffer by n bytes.
     */
    zxy_backend_read_nbytes_from_buffer_func read_nbytes;

    /**
     * Function for closing connection by force.
     */
    zxy_backend_close_callback_func force_close;

    /**
     * Check if specific event is ready or not.
     */
    zxy_backend_memebr_func is_ready_event;

    /**
     * Free allocated parameters during establishment of the connection.
     */
    zxy_backend_free_params free_params;

    /**
     * Params which is passed to each of above functions and is custom impelementation of your connection.
     */
    void *params;

    /**
     * Show different status during different state of the connection. 
     */
    int8_t set_free;
} zxy_backend_base_t;

/**
 * Backend plain connection properties.
 */
typedef struct 
{
    /**
     * Socket fd obtained during establishment of the connection.
     */
    int sock_fd;

    /**
     * Set when the connection closed.
     */
    int8_t is_closed;

    /**
     * Current events that set on the connection.
     */
    u_int32_t events;

    /**
     * Buffer manager which is controll buffer resize and nasty bit hacks.
     */
    zxy_buffer_manager_t *buffer_manager;

    /**
     * Show different status during different state of the connection. 
     */
    int8_t set_free;
} zxy_backend_conn_t;

/**
 * Backend ssl connection properties.
 */
typedef struct 
{
    /**
     * Socket fd obtained during establishment of the connection.
     */
    int sock_fd;

    /**
     * Set when the connection closed.
     */
    int8_t is_closed;

    /**
     * Current events that set on the connection.
     */
    u_int32_t events;

    /**
     * SSL Obj representing SSL states.
     */
    SSL *ssl;

    /**
     * BIO object for reading asynchronous.
     */
    BIO *rbio;

    /**
     * BIO object for writing asynchronous.
     */
    BIO *wbio;

    /**
     * Encrypt buffer manager controls data that is pass through ssl for encryption.
     */
    zxy_buffer_manager_t *encrypt_buffer_manager;

    /**
     * Buffer manager that is use for writing data in the socket.
     */
    zxy_buffer_manager_t *writing_buffer_manager;

    /**
     * Buffer manager that is use for reading data from the socket.
     */
    zxy_buffer_manager_t *read_buffer_manager;

    /**
     * Buffer manager that is use for reading plain data.
     */
    zxy_buffer_manager_t *plain_buffer_manager;

    /**
     * Show that when zroxy able to parse http request.
     */
    int8_t is_ssl_handshake_done;

    /**
     * Show different status during different state of the connection. 
     */
    int8_t set_free;
} zxy_backend_ssl_conn_t;

#endif /* BACKEND_TYPES_H */

