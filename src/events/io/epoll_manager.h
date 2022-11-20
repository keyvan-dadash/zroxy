/**
 * @file epoll_manager.h
 */

#ifndef EPOLL_MANAGER_H
#define EPOLL_MANAGER_H

#include <sys/epoll.h>

/**
 * FD for the epoll
 */
extern int epoll_fd;

typedef void (*zxy_event_loop_handler_func) (void*, int, uint32_t);

typedef void (*zxy_handler_free_params_func) (void*);

/**
 * Event hadnler releated to each socket fd.
 */
typedef struct 
{
    /**
     * Associated socket fd
     */
    int sock_fd;

    /**
     * Type of the socket(e.g. backend, client, or timer).
     */
    int8_t sock_type;

    /**
     * Callback func for handling this event.
     */
    zxy_event_loop_handler_func callback;

    /**
     * Params which is pass to the callback.
     */
    void *params;

    /**
     * Free allocated resource func.
     */
    zxy_handler_free_params_func free_params;
} zxy_event_handler_t;

/**
 * Link list for storing no longer use objects.
 */
typedef struct zxy_linklist_of_free_obj
{
    /**
     * Block which should be free.
     */
    void* block;

    /**
     * Next object which should be free.
     */
    struct zxy_linklist_of_free_obj* next;
} zxy_linklist_of_free_obj_t;

/**
 * Head of link list for the no longer use objects.
 */
extern zxy_linklist_of_free_obj_t *entry;

/**
 * Add old object to the link list.
 * \param ptr Pointer to the old object.
 */
void zxy_add_block_to_link_list(void *ptr);

/**
 * Intialize the epoll.
 */
void zxy_epoll_init();

/**
 * Add event hadnler to the epoll.
 * \param handler Handler of the event.
 * \param mask Events which cause the associated callback trigger.
 */
void zxy_add_handler_to_epoll(zxy_event_handler_t *handler, uint32_t mask);

/**
 * Remove the socket fd from the epoll.
 * \param sock_fd The socket fd.
 */
void zxy_remove_fd_from_epoll(int sock_fd);

/**
 * Start the event loop.
 */
void zxy_event_loop();

#endif /* EPOLL_MANAGER_H */

