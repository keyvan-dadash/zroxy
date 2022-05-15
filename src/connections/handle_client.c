#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "defines.h"
#include "connections/connections.h"
#include "events/io/epoll_manager.h"
#include "utils/net/netutils.h"
#include "utils/timer/timers.h"
#include "events/timers/timer_callback.h"

void zxy_handle_client_connection(int client_sock, char *backend_host, char *backend_port)
{
    struct addrinfo hints;
    struct addrinfo *addrs;
    struct addrinfo *addrs_iter;
    int getaddrinfo_error;

    int backend_socket_fd;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo_error = getaddrinfo(backend_host, backend_port, &hints, &addrs);

    if (getaddrinfo_error != 0) {
        fprintf(stderr, "cannot connect to backend: %s\n", gai_strerror(getaddrinfo_error));
        exit(-1);
    } 

    for (addrs_iter = addrs; 
         addrs_iter != NULL; 
         addrs_iter = addrs_iter->ai_next) {

        backend_socket_fd = socket(addrs_iter->ai_family, addrs_iter->ai_socktype, addrs_iter->ai_protocol);

        if (backend_socket_fd == -1) 
            continue;

        if (connect(backend_socket_fd, addrs_iter->ai_addr, addrs_iter->ai_addrlen) != -1)
            break;

        close(backend_socket_fd);
    }

    if (addrs_iter == NULL) {
        fprintf(stderr, "cannot connect to any backend");
    }

    freeaddrinfo(addrs);

    zxy_make_socket_nonblock(backend_socket_fd);

    //now we have both client and backend sockets in non block mode

    zxy_proxy_connection_t *proxy_obj = zxy_make_proxy_connection(client_sock, SSL_CONN, backend_socket_fd, SSL_CONN);
    
    //create and register client
    zxy_event_handler_t *client_handler = make_proxy_event_handler(proxy_obj, client_sock, CLIENT_SOCK);
    proxy_obj->client_handler_ptr = client_handler;

    //TODO: wtffff just happend about edge trigger
    zxy_add_handler_to_epoll(client_handler, EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLET);

    //create and register backend
    zxy_event_handler_t *backend_handler = make_proxy_event_handler(proxy_obj, backend_socket_fd, BACKEND_SOCK);
    proxy_obj->backend_handler_ptr = backend_handler;
    zxy_add_handler_to_epoll(backend_handler, EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR | EPOLLET);

    //create and register timer
    int32_t tfd = zxy_create_timer_with_expiration(10, 0);
    zxy_event_handler_t *timer_handler = make_proxy_event_handler(proxy_obj, tfd, PROXY_TIMER_SOCK);
    proxy_obj->timer_handler_ptr = timer_handler;
    zxy_add_handler_to_epoll(timer_handler, EPOLLIN | EPOLLHUP | EPOLLERR);
}

