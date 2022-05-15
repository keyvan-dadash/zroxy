#ifndef SSL_BACKEND_CALLBACK
#define SSL_BACKEND_CALLBACK

#include "events/io/epoll_manager.h"
#include "utils/io/io_helper.h"

int zxy_on_backend_ssl_read_event(void *ptr);

int zxy_on_backend_ssl_write_event(void *ptr, zxy_write_io_req_t* write_req);

int zxy_on_backend_ssl_close_event(void *ptr);

zxy_write_io_req_t zxy_backend_ssl_request_buffer_reader(void *ptr);

int zxy_backend_ssl_read_nbytes_from_buffer(void *ptr, int nbytes);

int zxy_backend_ssl_force_close(void *ptr);

int zxy_backend_ssl_is_ready_for_event(u_int32_t events, u_int32_t is_ready, void* ptr);

void zxy_free_backend_ssl(void *ptr);

#endif /* SSL_BACKEND_CALLBACK */

