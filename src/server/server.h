#ifndef SERVER_H
#define SERVER_H

#include "events/io/epoll_manager.h"

/**
 * Holds backend address for furthur connection.
 */
typedef struct 
{
    /**
     * Host of the backend to connect to.
     */
    char *backend_host;

    /**
     * Port of the backend to connect to.
     */
    char *backend_port;
} zxy_backend_addrs_t;

/**
 * Start to accepting new connections.
 * \param listen_fd The FD of the listen socket.
 * \param addrs The address of the backend for connect to.
 */
void zxy_accept_new_conn(int listen_fd, zxy_backend_addrs_t *addrs);

/**
 * Handle listener socket event which trigger when there are some clients waiting to be accepted.
 * \param ptr Pointer to the event handler object.
 * \param listen_fd The FD of the listener socket.
 * \param events The events that has been set on the listener FD.
 */
void zxy_handle_accepting_connections(void *ptr, int listen_fd, uint32_t events);

/**
 * Start the zroxy server.
 * \param listen_fd The FD of the listener socket.
 * \param addrs Pointer to the backend address struct.
 */
void zxy_start_server(int listen_fd, zxy_backend_addrs_t *addrs);

#endif /* SERVER_H */

