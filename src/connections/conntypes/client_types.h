#ifndef CLIENT_TYPES_H
#define CLIENT_TYPES_H

#include <sys/types.h>

#include <openssl/ssl.h>
#include <openssl/bio.h>

#include "utils/io/buffer_manager.h"
#include "utils/io/io_helper.h"

typedef int (*zxy_client_read_callback_func)(void*);

typedef int (*zxy_client_write_callback_func)(void*, zxy_write_io_req_t*);

typedef int (*zxy_client_close_callback_func)(void*);

typedef zxy_write_io_req_t (*zxy_client_request_buffer_reader)(void*);

typedef int (*zxy_client_read_nbytes_from_buffer_func)(void*, int32_t);

typedef void (*zxy_client_event_callback)(int32_t, u_int32_t, void*);

typedef int  (*zxy_client_memebr_func)(u_int32_t, u_int32_t, void*);

typedef void (*zxy_client_free_params)(void*);

/**
 * client base connection which is all other type of client connection should implement this
 */
typedef struct 
{
    /**
     * client callback functions
     */
    zxy_client_read_callback_func on_read;
    zxy_client_write_callback_func on_write;
    zxy_client_close_callback_func on_close;

    /**
     * client extra function for controll object
     */
    zxy_client_request_buffer_reader request_buffer_reader;
    zxy_client_read_nbytes_from_buffer_func read_nbytes;
    zxy_client_close_callback_func force_close;
    zxy_client_memebr_func is_ready_event;
    zxy_client_free_params free_params;

    /**
     * params which is passed to each of above functions
     * 
     */
    void *params;

    /**
     * should free?
     */
    int8_t set_free;
} zxy_client_base_t;

/**
 * client plain connection
 */
typedef struct 
{
    /**
     * sock fd properties
     */
    int sock_fd;
    int8_t is_closed;
    u_int32_t events;

    /**
     * buffer manager which is controll buffer resize and nasty bit hacks
     */
    zxy_buffer_manager_t *buffer_manager;

    /**
     * should free?
     */
    int8_t set_free;

} zxy_client_conn_t;

/**
 * client ssl connection
 */
typedef struct 
{
    /**
     * sock fd properties
     */
    int sock_fd;
    int8_t is_closed;
    u_int32_t events;

    /**
     * SSL Obj
     */
    SSL *ssl;

    /**
     * BIO objects for reading and writing non block
     */
    BIO *rbio;
    BIO *wbio;

    /**
     * encrypt buffer manager which is controll data which is should go through ssl for encryption
     */
    zxy_buffer_manager_t *encrypt_buffer_manager;

    /**
     * buffer which is should write to sockets
     */
    zxy_buffer_manager_t *writing_buffer_manager;

    /**
     * buffer which is should read
     */
    zxy_buffer_manager_t *read_buffer_manager;

    /**
     * this where other peer read unencrypt data
     */
    zxy_buffer_manager_t *plain_buffer_manager;

    /**
     * Decide when we should start to parse http requests
     */
    int8_t is_ssl_handshake_done;

    /**
     * should free?
     */
    int8_t set_free;

} zxy_client_ssl_conn_t;

#endif /* CLIENT_TYPES_H */

