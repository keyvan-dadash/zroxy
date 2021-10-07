


#ifndef CLIENT_CALLBACKS_H
#define CLIENT_CALLBACKS_H

#include "events/io/epoll_manager.h"
#include "utils/io/io_helper.h"

int zxy_on_client_plain_read_event(void *ptr);

int zxy_on_client_plain_write_event(void *ptr, zxy_write_io_req_t* write_req);

int zxy_on_client_plain_close_event(void *ptr);

zxy_write_io_req_t zxy_client_plain_request_buffer_reader(void *ptr);

int zxy_client_plain_force_close(void *ptr);

int zxy_client_plain_is_ready_for_event(u_int32_t events, u_int32_t is_ready, void* ptr);

#endif /* CLIENT_CALLBACKS_H */