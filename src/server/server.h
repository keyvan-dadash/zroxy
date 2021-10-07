


#ifndef SERVER_H
#define SERVER_H


#include "events/io/epoll_manager.h"


typedef struct 
{
    char *backend_host;
    char *backend_port;
} zxy_backend_addrs_t;

void zxy_accept_new_conn(int listen_fd, zxy_backend_addrs_t *addrs);

void zxy_handle_accepting_connections(void *ptr, int listen_fd, uint32_t events);

void zxy_start_server(int listen_fd, zxy_backend_addrs_t *addrs);



#endif /* SERVER_H */