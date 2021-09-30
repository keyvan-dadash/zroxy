


#ifndef SERVER_H
#define SERVER_H


#include "epoll_manager.h"


typedef struct 
{
    char *backend_host;
    char *backend_port;
} backend_addrs_t;

void accept_new_conn(int listen_fd, backend_addrs_t *addrs);

void handle_accepting_connections(int listen_fd, uint32_t events, void *ptr);

void start_server(int listen_fd, backend_addrs_t *addrs);



#endif /* SERVER_H */