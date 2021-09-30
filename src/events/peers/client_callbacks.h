


#ifndef CLIENT_CALLBACKS_H
#define CLIENT_CALLBACKS_H

#include "epoll_manager.h"


void on_client_read_event(void *ptr);

void on_client_write_event(void *ptr);

void on_client_close_event(void *ptr);

void client_on_event_callback(int client_sock_fd, uint32_t events, void *ptr);

#endif /* CLIENT_CALLBACKS_H */