#ifndef TIMER_CALLBACK_H
#define TIMER_CALLBACK_H

#include <sys/types.h>
#include <sys/epoll.h>

#include "events/io/epoll_manager.h"
#include "connections/conntypes/proxy_types.h"

zxy_event_handler_t* zxy_make_timer_handler(int32_t tfd, zxy_proxy_connection_t *proxy_handler);

void zxy_free_timer_handler(zxy_event_handler_t *timer_handler);

void zxy_on_timer_event_callback(int32_t fd, uint32_t event, void *ptr);

void zxy_free_timer_params(void *ptr);

#endif /* TIMER_CALLBACK_H */

