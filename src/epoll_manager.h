

#ifndef EPOLL_MANAGER_H
#define EPOLL_MANAGER_H

#include <sys/epoll.h>


extern int epoll_fd;

typedef void (*func) (uint32_t);

typedef struct 
{
    int sock_fd;
    func callback;
    void (*handle) (uint32_t events);
} handler_t;


void epoll_init();

void add_fd_to_epoll(handler_t *handler, uint32_t event_mask);

void remove_fd_from_epoll(int sock_fd);

void event_loop();

#endif /* EPOLL_MANAGER_H */