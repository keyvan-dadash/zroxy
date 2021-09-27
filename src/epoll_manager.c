


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/epoll.h>

#include "logs.h"
#include "epoll_manager.h"
#include "defines.h"


int epoll_fd = -1;

void epoll_init()
{
    epoll_fd = epoll_create1(0);

    if (epoll_fd == -1) {
        LOG_FATAL(-1, "cannot create epoll fd");
    }
}

void add_handler_to_epoll(handler_t *handler, uint32_t mask)
{
    struct epoll_event event;
    event.data.fd = handler->sock_fd;
    event.events = mask;
    event.data.ptr = handler;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event) == -1) {
        LOG_ERROR("Cannot add client fd(%d) to epoll set", event.data.fd);
    }
}

void remove_fd_from_epoll(int sock_fd)
{
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock_fd, NULL);
}

void event_loop(int server_fd)
{
    struct epoll_event events[MAX_EVENTS];

    while (1) {
        epoll_wait(epoll_fd, events, MAX_EVENTS, 30000);

        for (int i = 0; i < MAX_EVENTS; i++) {

            handler_t *handler = (handler_t*) events[i].data.ptr;

            handler->callback(events[i].data.fd, events[i].events, handler->params);
        }
    }
}