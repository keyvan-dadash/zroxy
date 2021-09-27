


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

#include "server.h"
#include "logs.h"
#include "handle_client.h"
#include "netutils.h"

void accept_new_conn(int listen_fd, backend_addrs_t *addrs)
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

        
        make_socket_nonblock(new_client_sock_fd);
        handle_client_connection(new_client_sock_fd, addrs->backend_host, addrs->backend_port);
    }
}

void free_listen_fd_requirments(void *ptr)
{
    // backend_addrs_t *addrs = (backend_addrs_t*)ptr;

    // free(addrs->backend_host);
    // free(addrs->backend_port);
}

void handle_accepting_connections(int listen_fd, uint32_t events, void *ptr)
{
    backend_addrs_t *addrs = (backend_addrs_t*)ptr;

    //TODO: free backend_addrs_t buffers

    if ((events & EPOLLHUP) | (events & EPOLLERR)) {//Error or close
        // LOG_WARNING("Listen fd is going to close and remove from epoll");
        remove_fd_from_epoll(listen_fd);
        close(listen_fd);
        free_listen_fd_requirments(ptr);
        return;
    }

    if (events & EPOLLIN) { //READ ready
        accept_new_conn(listen_fd, addrs);
    }

    if (events & EPOLLOUT) { //Write ready
        LOG_WARNING("Listen fd is in out mode???");
    }
}


void start_server(int listen_fd, backend_addrs_t *addrs)
{
    handler_t *listen_fd_handler = malloc(sizeof(handler_t));
    listen_fd_handler->callback = handle_accepting_connections;
    listen_fd_handler->params = addrs;
    listen_fd_handler->sock_fd = listen_fd;
    listen_fd_handler->free_params = free_listen_fd_requirments;

    add_handler_to_epoll(listen_fd_handler, EPOLLIN | EPOLLERR | EPOLLHUP);

    event_loop();
}