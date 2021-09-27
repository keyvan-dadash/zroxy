


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <errno.h>

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
    memset(&event, 0, sizeof(struct epoll_event));

    event.events = mask;
    event.data.ptr = handler;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, handler->sock_fd, &event) == -1) {
        LOG_ERROR("Cannot add client fd(%d) to epoll set\n", handler->sock_fd);
        return;
    }

    LOG_INFO("fd(%d) added\n", handler->sock_fd);
}

void remove_fd_from_epoll(int sock_fd)
{
    int error = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock_fd, NULL);

    if (error == -1)
        LOG_ERROR("Cannot remove fd(%d) from epoll(%d) because of error: %s\n", sock_fd, epoll_fd, strerror(errno));
}

void event_loop(int server_fd)
{
    int nfd;
    struct epoll_event events[MAX_EVENTS];

    while (1) {
        LOG_INFO("fuck1\n");
        nfd = epoll_wait(epoll_fd, events, MAX_EVENTS, 30000);

        if (nfd == 0) {
            continue;
        } else if (nfd == -1) {
            LOG_FATAL(-1, "Epoll crashed: %s", strerror(errno));
        }

        LOG_INFO("fuck\n");

        for (int i = 0; i < nfd; i++) {

            handler_t *handler = (handler_t*) events[i].data.ptr;

            handler->callback(handler->sock_fd, events[i].events, handler->params);
        }
    }
}