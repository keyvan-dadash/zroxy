

#include <stdlib.h>
#include <sys/types.h>
#include <memory.h>


#include "defines.h"

#include "connections/client_conn_req.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/client_types.h"
#include "events/peers/client_callbacks.h"

#include "utils/io/buffer_manager.h"




zxy_client_base_t* zxy_make_client_base_conn(void *params)
{
    zxy_client_base_t *client_base_conn = (zxy_client_base_t*)calloc(1, sizeof(zxy_client_base_t));

    client_base_conn->params = params;

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



// make_client_conn(zxy_proxy_connection_t *proxy_conn, int client_sock_fd)
// {
//     zxy_client_base_t client_base_info;
//     client_base_info.client_sock_fd = client_sock_fd;
//     client_base_info.is_client_closed = 0;
//     client_base_info.max_bufer_size = READ_BUF_SIZE + 1;
//     client_base_info.read_buf = (char *)malloc(sizeof(char) * client_base_info.max_bufer_size);
//     client_base_info.buffer_ptr = 0;
//     client_base_info.client_events = 0;
//     client_base_info.set_free = 0;

//     proxy_handler->client_info = client_base_info;

//     proxy_handler->client_info.client_handlers = (client_handlers_t *)malloc(sizeof(client_handlers_t));
//     proxy_handler->client_info.client_handlers->on_read = on_client_read_event;
//     proxy_handler->client_info.client_handlers->on_write = on_client_write_event;
//     proxy_handler->client_info.client_handlers->on_close = on_client_close_event;

//     handler_t *handler = (handler_t *)malloc(sizeof(handler_t));
//     handler->sock_fd = client_sock_fd;
//     handler->params = proxy_handler;
//     handler->free_params = free_client_requirments;
//     handler->callback = client_on_event_callback;

//     return handler;
// }