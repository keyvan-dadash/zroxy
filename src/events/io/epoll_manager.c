


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <errno.h>

#include "defines.h"
#include "logging/logs.h"
#include "events/io/epoll_manager.h"


int epoll_fd = -1;

zxy_linklist_of_free_obj_t *entry = NULL;

void zxy_add_block_to_link_list(void *ptr)
{
    zxy_linklist_of_free_obj_t *new_entry = (zxy_linklist_of_free_obj_t *)malloc(sizeof(zxy_linklist_of_free_obj_t));
    new_entry->block = ptr;
    new_entry->next = entry;
    entry = new_entry;
}

void zxy_epoll_init()
{
    epoll_fd = epoll_create1(0);

    if (epoll_fd == -1) {
        LOG_FATAL(-1, "cannot create epoll fd");
    }
}

void zxy_add_handler_to_epoll(zxy_event_handler_t *handler, uint32_t mask)
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

void zxy_remove_fd_from_epoll(int sock_fd)
{
    int error = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock_fd, NULL);

    if (error == -1)
        LOG_ERROR("Cannot remove fd(%d) from epoll(%d) because of error: %s\n", sock_fd, epoll_fd, strerror(errno));
}

void zxy_event_loop(int server_fd)
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

            zxy_event_handler_t *handler = (zxy_event_handler_t*) events[i].data.ptr;

            handler->callback(handler, handler->sock_fd, events[i].events);

        }

        zxy_linklist_of_free_obj_t *local_entry;

        while (entry != NULL) { //free handler objects
            zxy_event_handler_t *handler = (zxy_event_handler_t *)entry->block;
            handler->free_params(handler->params);
            LOG_INFO("fuckkkkkkkkkkkkkkkkkkkkkkkkkkk\n");
            free(entry->block);
            local_entry = entry->next;
            free(entry);
            entry = local_entry;
        }
    }
}