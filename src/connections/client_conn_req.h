


#ifndef CLIENT_CONN_REQ_H
#define CLIENT_CONN_REQ_H


#include "events/io/epoll_manager.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/client_types.h"


zxy_client_base_t* zxy_make_client_base_conn(void *params);


/////////// pain connection /////////////

zxy_client_conn_t* zxy_make_client_plain_conn(int sock_fd);

void zxy_set_up_client_plain_base_callbacks(zxy_client_base_t* client_base);

#endif /* CLIENT_CONN_REQ_H */