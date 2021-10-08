



#include <stdlib.h>
#include <sys/types.h>
#include <memory.h>


#include "defines.h"

#include "connections/backend_conn_req.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/backend_types.h"
#include "events/peers/backend_callbacks.h"

#include "utils/io/buffer_manager.h"


zxy_backend_base_t* zxy_make_backend_base_conn(void *params)
{
    zxy_backend_base_t *backend_base_conn = (zxy_backend_base_t*)calloc(1, sizeof(zxy_backend_base_t));

    backend_base_conn->params = params;
    backend_base_conn->set_free = 0;

    return backend_base_conn;
}


zxy_backend_conn_t* zxy_make_backend_plain_conn(int sock_fd)
{
    zxy_backend_conn_t *backend_conn = (zxy_backend_conn_t*)calloc(1, sizeof(zxy_backend_conn_t));

    backend_conn->sock_fd = sock_fd;
    backend_conn->is_closed = 0;
    backend_conn->set_free = 0;
    backend_conn->buffer_manager = zxy_malloc_buffer_manager_with_buffer_size(READ_BUF_SIZE + 1);

    return backend_conn;
}

void zxy_set_up_backend_plain_base_callbacks(zxy_backend_base_t* backend_base)
{
    backend_base->free_params = zxy_free_backend_plain;
    backend_base->force_close = zxy_backend_plain_force_close;
    backend_base->is_ready_event = zxy_backend_plain_is_ready_for_event;
    backend_base->on_close = zxy_on_backend_plain_close_event;
    backend_base->on_read = zxy_on_backend_plain_read_event;
    backend_base->on_write = zxy_on_backend_plain_write_event;
    backend_base->request_buffer_reader = zxy_backend_plain_request_buffer_reader;
}