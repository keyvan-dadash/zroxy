

#include <sys/types.h>
#include <stdlib.h>

#include "logs.h"
#include "epoll_manager.h"
#include "timers.h"
#include "timer_callback.h"

//TODO: backend and client closed and timer sill alive

handler_t* make_timer_handler(int32_t tfd, proxy_handler_t *proxy_handler)
{
    handler_t *handler = (handler_t *)malloc(sizeof(handler_t));

    handler->sock_fd = tfd;
    handler->callback = on_timer_event_callback;
    handler->free_params = free_timer_params;
    handler->params = (void *)proxy_handler;

    return handler;
}


void on_timer_event_callback(int32_t fd, uint32_t event, void *ptr)
{
    LOG_INFO("huhhhhhh?\n");
    proxy_handler_t *proxy_obj = (proxy_handler_t*)ptr;

    if (proxy_obj->timer_status_and_fd == RELAX_TIMER) {
        LOG_INFO("huhhhhhh1?\n");
        proxy_obj->timer_status_and_fd = TIMER_RELAXED;
        set_timer_time_with(fd, 1, 0);
    } else if ( (proxy_obj->timer_status_and_fd == TIMER_RELAXED) ||
                (proxy_obj->timer_status_and_fd > 0) ) 
    {
        LOG_INFO("huhhhhhh2?\n");
        proxy_obj->timer_status_and_fd = TIMER_IS_UP;
        remove_fd_from_epoll(fd);
        add_block_to_link_list(proxy_obj->timer_handler_ptr);

        if (!proxy_obj->backend_info.is_backend_closed)
            proxy_obj->backend_info.backend_handlers->on_close(ptr);

        if (!proxy_obj->client_info.is_client_closed)   
            proxy_obj->client_info.client_handlers->on_close(ptr);
    }
}


void free_timer_params(void *ptr)
{
    LOG_INFO("free timer\n");
}