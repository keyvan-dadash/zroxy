/**
 * @file proxy_callbacks.h
 */

#ifndef PROXY_CALLBACKS
#define PROXY_CALLBACKS

#include <sys/types.h>

/**
 * Callback when a connection has a event to handle.
 * \param handler Pointer to the zxy_event_handler_t.
 * \param sock_fd The sock_fd which events has been set on.
 * \param events The events that need to be handle.
 */
void zxy_proxy_events_callback(void* handler, int sock_fd, u_int32_t events);

/**
 * Free allocated resources on the proxy object.
 * \param params Pointer to the zxy_proxy_connection_t.
 */
void zxy_free_proxy_obj(void* params);

#endif /* PROXY_CALLBACKS */

