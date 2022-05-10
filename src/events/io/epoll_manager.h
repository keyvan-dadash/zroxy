#ifndef EPOLL_MANAGER_H
#define EPOLL_MANAGER_H

#include <sys/epoll.h>

extern int epoll_fd;

typedef void (*zxy_event_loop_handler_func) (void*, int, uint32_t);

typedef void (*zxy_handler_free_params_func) (void*);

typedef struct 
{
    int sock_fd;
    int8_t sock_type;
    zxy_event_loop_handler_func callback;
    void *params;
    zxy_handler_free_params_func free_params;
} zxy_event_handler_t;

typedef struct zxy_linklist_of_free_obj
{
    void* block;
    struct zxy_linklist_of_free_obj* next;
} zxy_linklist_of_free_obj_t;

extern zxy_linklist_of_free_obj_t *entry;

void zxy_add_block_to_link_list(void *ptr);

void zxy_epoll_init();

void zxy_add_handler_to_epoll(zxy_event_handler_t *handler, uint32_t mask);

void zxy_remove_fd_from_epoll(int sock_fd);

void zxy_event_loop();

#endif /* EPOLL_MANAGER_H */

