


#ifndef CLIENT_SSL_CONN_REQ_H
#define CLIENT_SSL_CONN_REQ_H



#include "events/io/epoll_manager.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/client_types.h"


zxy_client_base_t* zxy_make_ssl_client_base_conn(void *params);


/////////// ssl connection /////////////

zxy_client_ssl_conn_t* zxy_make_client_ssl_conn(SSL_CTX *ctx, int sock_fd);

void zxy_set_up_client_ssl_base_callbacks(zxy_client_base_t* client_base);




#endif /* CLIENT_SSL_CONN_REQ_H */