/**
 * @file timer_callback.h
 */

#ifndef TIMER_CALLBACK_H
#define TIMER_CALLBACK_H

#include <sys/types.h>
#include <sys/epoll.h>

#include "events/io/epoll_manager.h"
#include "connections/conntypes/proxy_types.h"

/**
 * Create timer handler object.
 * \param tfd The associated FD with timer.
 * \param proxy_handler The releated proxy handler object to this timer.
 * \return The event handler which should submit on the epoll.
 */
zxy_event_handler_t* zxy_make_timer_handler(int32_t tfd, zxy_proxy_connection_t *proxy_handler);

/**
 * Remove timer fd from the epoll.
 * \param timer_handler Pointer to the handler of timer.
 */
void zxy_free_timer_handler(zxy_event_handler_t *timer_handler);

/**
 * Callback when timer is up.
 * \param fd The FD realated to timer.
 * \param event The event that has been set on the fd.
 * \param ptr Pointer to the handler of the timer.
 */
void zxy_on_timer_event_callback(int32_t fd, uint32_t event, void *ptr);

/**
 * Free the allocated resources.
 * \param ptr Pointer to the handler of the timer.
 */
void zxy_free_timer_params(void *ptr);

#endif /* TIMER_CALLBACK_H */

