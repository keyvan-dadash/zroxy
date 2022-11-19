#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include "events/io/epoll_manager.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/client_types.h"
#include "connections/conntypes/backend_types.h"
#include "connections/conntypes/proxy_types.h"

/**
 * Make a event handler for submit in epoll.
 * \param proxy_obj The proxy connection settings that apply to the connection.
 * \param sock The socket fd.
 * \param sock_type The type of socket.
 * \return zxy_event_handler_t Pointer to the event handler struct that contains necessery infos.
 */
zxy_event_handler_t* make_proxy_event_handler(zxy_proxy_connection_t* proxy_obj, int sock, int8_t sock_type);

/**
 * Make proxy connection based on connection type for each side.
 * \param client_fd Obtained fd for the client.
 * \param client_conn_type Type of connection for the client(e.g. ssl or plain).
 * \param backend_fd Obtained fd for the backend.
 * \param backend_conn_type Type of connection for the backend(e.g. ssl or plain).
 * \return zxy_proxy_connection_t Pointer to the proxy connection struct which contains proxy settings for each side.
 */
zxy_proxy_connection_t* zxy_make_proxy_connection(
            int32_t client_fd,
            enum zxy_conn_type client_conn_type, 
            int32_t backend_fd,
            enum zxy_conn_type backend_conn_type);

/**
 * Create client connection with specified connection type.
 * \param client_fd Obtained fd for the client.
 * \param client_conn_type Type of the connection for the client.
 * \return zxy_client_base_t Pointer to the client base struct which holds client connection settings.
 */
zxy_client_base_t* zxy_make_client_conn_with_type(int32_t client_fd, enum zxy_conn_type client_conn_type);

/**
 * Create backend connection with specified connection type.
 * \param backend_fd Obtained fd for the backend.
 * \param backend_conn_type Type of the connection for the backend.
 * \return zxy_backend_base_t Pointer to the backend base struct which holds backend connection settings.
 */
zxy_backend_base_t* zxy_make_backend_conn_with_type(int32_t backend_fd, enum zxy_conn_type backend_conn_type);

#endif /* CONNECTIONS_H */

