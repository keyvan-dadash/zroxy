



#include "defines.h"
#include "events/io/epoll_manager.h"
#include "events/peers/proxy_callbacks.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/client_types.h"
#include "connections/conntypes/backend_types.h"
#include "utils/io/io_helper.h"



zxy_backend_base_t* get_backend_base_from(void *ptr)
{
    return ((zxy_proxy_connection_t*)((zxy_event_handler_t*)ptr)->params)->backend;
}

zxy_client_base_t* get_client_base_from(void *ptr)
{
    return ((zxy_proxy_connection_t*)((zxy_event_handler_t*)ptr)->params)->client;
}

void zxy_handle_client_events(
    zxy_backend_base_t *backend_base, 
    zxy_client_base_t *client_base, 
    u_int32_t events)
{

    if (client_base->is_ready_event(events, CLOSE_EVENT, client_base->params)) {
        //TODO: handle close
        return;
    }

    if (client_base->is_ready_event(events, WRITE_EVENT, client_base->params)) {
        zxy_write_io_req_t write_req = backend_base->request_buffer_reader(backend_base->params);

        if (write_req.send_nbytes > 0)
            client_base->on_write((void*)client_base->params, &write_req);
    }

    if (client_base->is_ready_event(events, READ_EVENT, client_base->params)) {
        int read_bytes = client_base->on_read((void*)client_base->params);

        if (read_bytes > 0 && backend_base->is_ready_event(-1, WRITE_EVENT, backend_base->params)) {
            zxy_write_io_req_t write_req = client_base->request_buffer_reader(client_base->params);

            backend_base->on_write((void*)backend_base->params, &write_req);
        }
    }


}

void zxy_handle_backend_events(
    zxy_backend_base_t *backend_base, 
    zxy_client_base_t *client_base, 
    u_int32_t events)
{
    if (backend_base->is_ready_event(events, CLOSE_EVENT, backend_base->params)) {
        //TODO: handle close
        return;
    }

    if (backend_base->is_ready_event(events, WRITE_EVENT, backend_base->params)) {
        zxy_write_io_req_t write_req = client_base->request_buffer_reader(client_base->params);

        if (write_req.send_nbytes > 0)
            backend_base->on_write((void*)backend_base->params, &write_req);
    }

    if (backend_base->is_ready_event(events, READ_EVENT, backend_base->params)) {
        int read_bytes = backend_base->on_read((void*)backend_base->params);

        if (read_bytes > 0 && client_base->is_ready_event(-1, WRITE_EVENT, client_base->params)) {
            zxy_write_io_req_t write_req = backend_base->request_buffer_reader(backend_base->params);

            client_base->on_write((void*)client_base->params, &write_req);
        }
    }
}

void zxy_proxy_events_callback(void* handler, int sock_fd, u_int32_t events)
{
    zxy_event_handler_t *event_handler = (zxy_event_handler_t*)handler;
    zxy_backend_base_t *backend_base = get_backend_base_from(handler);
    zxy_client_base_t *client_base = get_client_base_from(handler);

    switch (event_handler->sock_type)
    {
    case CLIENT_SOCK: {
        zxy_handle_client_events(
            backend_base,
            client_base,
            events
        );
        break;
    }

    case BACKEND_SOCK: {
        zxy_handle_backend_events(
            backend_base,
            client_base,
            events
        );
        break;
    }
    
    default:
        break;
    }
}

void zxy_free_proxy_obj(void* params)
{
    //TODO: handle some how free
}