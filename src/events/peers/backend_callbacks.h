


#ifndef BACKEND_CALLBACKS_H
#define BACKEND_CALLBACKS_H


#include "epoll_manager.h"


void on_backend_read_event(void *ptr);

void on_backend_write_event(void *ptr);

void on_backend_close_event(void *ptr);

void backend_on_event_callback(int client_sock_fd, uint32_t events, void *ptr);

#endif /* BACKEND_CALLBACKS_H */