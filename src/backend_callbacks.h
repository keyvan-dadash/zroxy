


#ifndef BACKEND_CALLBACKS_H
#define BACKEND_CALLBACKS_H


#include "epoll_manager.h"


void on_backend_read_event(backend_connection_info_t *backend_info, client_connection_info_t *client_info);

void on_backend_write_event(backend_connection_info_t *backend_info, client_connection_info_t *client_info);

void on_backend_close_event(backend_connection_info_t *backend_info);


#endif /* BACKEND_CALLBACKS_H */