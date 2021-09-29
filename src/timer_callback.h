


#ifndef TIMER_CALLBACK_H
#define TIMER_CALLBACK_H

#include <sys/types.h>
#include <sys/epoll.h>

#include "epoll_manager.h"


handler_t* make_timer_handler(int32_t tfd, proxy_handler_t *proxy_handler);

void on_timer_event_callback(int32_t fd, uint32_t event, void *ptr);


void free_timer_params(void *ptr);



#endif /* TIMER_CALLBACK_H */