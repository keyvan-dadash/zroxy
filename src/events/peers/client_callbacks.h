


#ifndef CLIENT_CALLBACKS_H
#define CLIENT_CALLBACKS_H

#include "epoll_manager.h"


int zxy_on_client_plain_read_event(void *ptr);

int zxy_on_client_plain_write_event(void *ptr);

int zxy_on_client_plain_close_event(void *ptr);

void zxy_client_on_event_callback(int client_sock_fd, uint32_t events, void *ptr);

#endif /* CLIENT_CALLBACKS_H */