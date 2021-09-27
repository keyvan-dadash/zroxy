


#ifndef CLIENT_CALLBACKS_H
#define CLIENT_CALLBACKS_H

#include "epoll_manager.h"


void on_client_read_event(client_connection_info_t *client_handler, backend_connection_info_t *backend_handler);

void on_client_write_event(client_connection_info_t *client_handler, backend_connection_info_t *backend_handler);

void on_client_close_event(client_connection_info_t *client_handlers);


#endif /* CLIENT_CALLBACKS_H */