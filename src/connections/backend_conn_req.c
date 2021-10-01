



#include <stdlib.h>
#include <sys/types.h>
#include <memory.h>


#include "defines.h"

#include "connections/backend_conn_req.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/backend_types.h"

#include "utils/io/buffer_mamager.h"


zxy_backend_base_t* zxy_make_backend_base_conn(void *params)
{
    zxy_backend_base_t *backend_base_conn = (zxy_backend_base_t*)calloc(1, sizeof(zxy_backend_base_t));

    backend_base_conn->params = params;

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

void zxy_set_up_backend_plain_conn_callbacks(zxy_backend_conn_t* backend_conn)
{

}
// handler_t* make_backend_handler(zxy_proxy_connection_t *proxy_handler, int backend_sock_fd)
// {
//     zxy_backend_base_t backend_conn_info;
//     backend_conn_info.backend_sock_fd = backend_sock_fd;
//     backend_conn_info.is_backend_closed = 0;
//     backend_conn_info.max_bufer_size = READ_BUF_SIZE + 1;
//     backend_conn_info.read_buf = (char *)malloc(sizeof(char) * backend_conn_info.max_bufer_size);
//     backend_conn_info.buffer_ptr = 0;
//     backend_conn_info.backend_events = 0;
//     backend_conn_info.set_free = 0;

//     proxy_handler->backend_info = backend_conn_info;

//     proxy_handler->backend_info.backend_handlers = (backend_handlers_t *)malloc(sizeof(backend_handlers_t));
//     proxy_handler->backend_info.backend_handlers->on_read = on_backend_read_event;
//     proxy_handler->backend_info.backend_handlers->on_write = on_backend_write_event;
//     proxy_handler->backend_info.backend_handlers->on_close = on_backend_close_event;

//     handler_t *handler = (handler_t *)malloc(sizeof(handler_t));
//     handler->sock_fd = backend_sock_fd;
//     handler->params = proxy_handler;
//     handler->free_params = free_backend_requirments;
//     handler->callback = backend_on_event_callback;

//     return handler;
// }