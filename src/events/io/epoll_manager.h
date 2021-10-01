

#ifndef EPOLL_MANAGER_H
#define EPOLL_MANAGER_H

#include <sys/epoll.h>

extern int epoll_fd;



typedef void (*func) (int, uint32_t, void*);

typedef void (*free_params_func) (void*);

typedef struct 
{
    int sock_fd;
    func callback;
    void *params;
    free_params_func free_params;
} handler_t;



typedef struct linklist_of_free_obj
{
    void* block;
    struct linklist_of_free_obj* next;
} linklist_of_free_obj_t;

extern linklist_of_free_obj_t *entry;

void add_block_to_link_list(void *ptr);

void epoll_init();

void add_handler_to_epoll(handler_t *handler, uint32_t mask);

void remove_fd_from_epoll(int sock_fd);

void event_loop();

#endif /* EPOLL_MANAGER_H */