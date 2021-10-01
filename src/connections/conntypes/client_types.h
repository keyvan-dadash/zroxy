


#ifndef CLIENT_TYPES_H
#define CLIENT_TYPES_H

#include <sys/types.h>

#include "utils/io/buffer_mamager.h"

typedef void (*zxy_client_callback_func)(void*);

typedef void (*zxy_client_event_callback)(int32_t, u_int32_t, void*);

typedef int  (*zxy_client_memebr_func)(u_int32_t, void*);

/**
 * client base connection which is all other type of client connection should implement this
 */
typedef struct 
{
    /**
     * client callback functions
     */
    zxy_client_callback_func on_read;
    zxy_client_callback_func on_write;
    zxy_client_callback_func on_close;


    /**
     * client extra function for controll object
     */
    zxy_client_callback_func force_close;
    zxy_client_memebr_func is_ready_event;

    /**
     * general callback function which is responsible for handling events
     */ 
    zxy_client_event_callback callback;


    /**
     * params which is passed to each of above functions
     * 
     */
    void *params;
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





#endif /* CLIENT_TYPES_H */