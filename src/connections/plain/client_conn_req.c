#include <stdlib.h>
#include <sys/types.h>
#include <memory.h>

#include "defines.h"

#include "connections/plain/client_conn_req.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/client_types.h"
#include "events/peers/plain/client_callbacks.h"

#include "utils/io/buffer_manager.h"

zxy_client_base_t* zxy_make_plain_client_base_conn(void *params)
{
    zxy_client_base_t *client_base_conn = (zxy_client_base_t*)calloc(1, sizeof(zxy_client_base_t));

    client_base_conn->params = params;
    client_base_conn->set_free = 0;

    return client_base_conn;
}

zxy_client_conn_t* zxy_make_client_plain_conn(int sock_fd)
{
    zxy_client_conn_t *client_conn = (zxy_client_conn_t*)calloc(1, sizeof(zxy_client_conn_t));

    client_conn->sock_fd = sock_fd;
    client_conn->is_closed = 0;
    client_conn->set_free = 0;
    client_conn->buffer_manager = zxy_malloc_buffer_manager_with_buffer_size(READ_BUF_SIZE + 1);

    return client_conn;
}

void zxy_set_up_client_plain_base_callbacks(zxy_client_base_t* client_base)
{
    client_base->free_params = zxy_free_client_plain;
    client_base->force_close = zxy_client_plain_force_close;
    client_base->is_ready_event = zxy_client_plain_is_ready_for_event;
    client_base->on_close = zxy_on_client_plain_close_event;
    client_base->on_read = zxy_on_client_plain_read_event;
    client_base->on_write = zxy_on_client_plain_write_event;
    client_base->request_buffer_reader = zxy_client_plain_request_buffer_reader;
}

