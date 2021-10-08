


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "server/server.h"
#include "logging/logs.h"
#include "connections/handle_client.h"
#include "utils/net/netutils.h"

void zxy_accept_new_conn(int listen_fd, zxy_backend_addrs_t *addrs)
{
    int new_client_sock_fd;

    while (1) {

        new_client_sock_fd = accept(listen_fd, NULL, NULL);

        if (new_client_sock_fd == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)) )
            break;
        else if(new_client_sock_fd == -1)
            LOG_FATAL(-1, "Something bad happend with accepting new client in listen_fd(%d) with error: %s\n", 
                            listen_fd, strerror(errno));

        LOG_INFO("accept new connection with fd(%d)\n", new_client_sock_fd);

        
        zxy_make_socket_nonblock(new_client_sock_fd);
        zxy_handle_client_connection(new_client_sock_fd, addrs->backend_host, addrs->backend_port);
    }
}

void zxy_free_listen_fd_requirments(void *ptr)
{
    
}

void zxy_handle_accepting_connections(void *ptr, int listen_fd, uint32_t events)
{
    zxy_event_handler_t *event_handler = (zxy_event_handler_t*)ptr;
    zxy_backend_addrs_t *addrs = (zxy_backend_addrs_t*)(event_handler->params);

    if ((events & EPOLLHUP) | (events & EPOLLERR)) {//Error or close
        // LOG_WARNING("Listen fd is going to close and remove from epoll");
        zxy_remove_fd_from_epoll(listen_fd);
        close(listen_fd);
        zxy_free_listen_fd_requirments(ptr);
        return;
    }

    if (events & EPOLLIN) { //READ ready
        zxy_accept_new_conn(listen_fd, addrs);
    }

    if (events & EPOLLOUT) { //Write ready
        LOG_WARNING("Listen fd is in out mode???");
    }
}


void zxy_start_server(int listen_fd, zxy_backend_addrs_t *addrs)
{
    zxy_event_handler_t *listen_fd_handler = (zxy_event_handler_t *)malloc(sizeof(zxy_event_handler_t));
    listen_fd_handler->callback = zxy_handle_accepting_connections;
    listen_fd_handler->params = addrs;
    listen_fd_handler->sock_fd = listen_fd;
    listen_fd_handler->free_params = zxy_free_listen_fd_requirments;

    zxy_add_handler_to_epoll(listen_fd_handler, EPOLLIN | EPOLLERR | EPOLLHUP);

    zxy_event_loop();
}