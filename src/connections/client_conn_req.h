


#ifndef CLIENT_CONN_REQ_H
#define CLIENT_CONN_REQ_H



#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/client_types.h"

zxy_client_base_t* zxy_make_client_base_conn(void *params);


/////////// pain connection /////////////

zxy_client_conn_t* zxy_make_client_plain_conn(int sock_fd);

void zxy_set_up_client_plain_conn_callbacks(zxy_client_conn_t* client_conn);

#endif /* CLIENT_CONN_REQ_H */