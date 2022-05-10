#include <sys/types.h>
#include <stdlib.h>

#include "logging/logs.h"
#include "events/io/epoll_manager.h"
#include "utils/timer/timers.h"
#include "events/timers/timer_callback.h"

//TODO: backend and client closed and timer sill alive

zxy_event_handler_t* zxy_make_timer_handler(int32_t tfd, zxy_proxy_connection_t *proxy_handler)
{
    zxy_event_handler_t *handler = (zxy_event_handler_t *)malloc(sizeof(zxy_event_handler_t));

    handler->sock_fd = tfd;
    handler->params = (void *)proxy_handler;

    return handler;
}

void zxy_free_timer_handler(zxy_event_handler_t *timer_handler)
{
    zxy_remove_fd_from_epoll(timer_handler->sock_fd);
    zxy_add_block_to_link_list(timer_handler);
}

void zxy_on_timer_event_callback(int32_t fd, uint32_t event, void *ptr)
{
    
}

void zxy_free_timer_params(void *ptr)
{
    LOG_INFO("free timer\n");
}

