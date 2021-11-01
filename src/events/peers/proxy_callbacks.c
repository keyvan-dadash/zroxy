


#include <stdlib.h>

#include "defines.h"
#include "logging/logs.h"
#include "events/io/epoll_manager.h"
#include "events/peers/proxy_callbacks.h"
#include "events/timers/timer_callback.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/client_types.h"
#include "connections/conntypes/backend_types.h"
#include "utils/io/io_helper.h"
#include "utils/timer/timers.h"



zxy_backend_base_t* get_backend_base_from(void *ptr)
{
    return ((zxy_proxy_connection_t*)((zxy_event_handler_t*)ptr)->params)->backend;
}

zxy_client_base_t* get_client_base_from(void *ptr)
{
    return ((zxy_proxy_connection_t*)((zxy_event_handler_t*)ptr)->params)->client;
}

zxy_proxy_connection_t* get_proxy_conn_from(void *ptr)
{
    return ((zxy_proxy_connection_t*)((zxy_event_handler_t*)ptr)->params);
}

int zxy_check_backend_close(
    zxy_proxy_connection_t *proxy_conn,
    zxy_backend_base_t *backend_base, 
    zxy_client_base_t *client_base
)
{
    //check if backend closed
    if (backend_base->set_free == 1) {
        
        //first free backend
        backend_base->free_params(backend_base);

        //close client
        client_base->force_close(client_base);
        client_base->free_params(client_base);

        //free both clinet and backend
        zxy_add_block_to_link_list(proxy_conn->backend_handler_ptr);
        zxy_add_block_to_link_list(proxy_conn->client_handler_ptr);

        return 1;
    }

    return 0;
}


int zxy_check_client_close(
    zxy_proxy_connection_t *proxy_conn,
    zxy_backend_base_t *backend_base, 
    zxy_client_base_t *client_base
)
{
    //check if client closed
    if (client_base->set_free == 1) {
        
        //first free client
        client_base->free_params(client_base);

        //close backend
        backend_base->force_close(backend_base);
        backend_base->free_params(backend_base);

        //free both clinet and backend
        zxy_add_block_to_link_list(proxy_conn->backend_handler_ptr);
        zxy_add_block_to_link_list(proxy_conn->client_handler_ptr);

        return 1;
    }

    return 0;
}


void zxy_handle_client_events(
    zxy_proxy_connection_t *proxy_conn,
    zxy_backend_base_t *backend_base, 
    zxy_client_base_t *client_base, 
    u_int32_t events)
{

    if (client_base->is_ready_event(events, CLOSE_EVENT, client_base)) {
        client_base->on_close(client_base);
        // client_base->free_params(client_base);
        // LOG_INFO("close\n");
        return;
    }

    if (client_base->is_ready_event(events, READ_EVENT, client_base)) {
        if (zxy_check_backend_close(
            proxy_conn,
            backend_base,
            client_base
        )) return;

        int read_bytes = client_base->on_read((void*)client_base);

        // LOG_INFO("read %d\n", read_bytes);

        if (read_bytes > 0 && backend_base->is_ready_event(-1, WRITE_EVENT, backend_base)) {
            zxy_write_io_req_t write_req = client_base->request_buffer_reader(client_base);

            backend_base->on_write((void*)backend_base, &write_req);
        }
    }

    if (client_base->is_ready_event(events, WRITE_EVENT, client_base)) {
        zxy_write_io_req_t write_req = backend_base->request_buffer_reader(backend_base);

        // LOG_INFO("write\n");
        // if (write_req.send_nbytes > 0)
            client_base->on_write((void*)client_base, &write_req);
        
        zxy_check_backend_close(
            proxy_conn,
            backend_base,
            client_base
        );
    }

}

void zxy_handle_backend_events(
    zxy_proxy_connection_t *proxy_conn,
    zxy_backend_base_t *backend_base, 
    zxy_client_base_t *client_base, 
    u_int32_t events)
{
    if (backend_base->is_ready_event(events, CLOSE_EVENT, backend_base)) {
        backend_base->on_close(backend_base);
        // backend_base->free_params(backend_base);
        return;
    }

    if (backend_base->is_ready_event(events, READ_EVENT, backend_base)) {
        if (zxy_check_client_close(
            proxy_conn,
            backend_base,
            client_base
        )) return;

        int read_bytes = backend_base->on_read((void*)backend_base);

        if (read_bytes > 0 && client_base->is_ready_event(-1, WRITE_EVENT, client_base)) {
            zxy_write_io_req_t write_req = backend_base->request_buffer_reader(backend_base);

            client_base->on_write((void*)client_base, &write_req);
        }
    }

    if (backend_base->is_ready_event(events, WRITE_EVENT, backend_base)) {
        zxy_write_io_req_t write_req = client_base->request_buffer_reader(client_base);

        LOG_INFO("hoho:%d %s\n", write_req.send_nbytes, write_req.buffer);
        if (write_req.send_nbytes > 0)
            backend_base->on_write((void*)backend_base, &write_req);

        zxy_check_client_close(
            proxy_conn,
            backend_base,
            client_base
        );
    }
}

void zxy_handle_timer_up_events(
    zxy_proxy_connection_t *proxy_conn,
    zxy_backend_base_t *backend_base, 
    zxy_client_base_t *client_base
)
{
    if (!backend_base->set_free) {
        backend_base->force_close(backend_base);
        backend_base->free_params(backend_base);
    }

    if (!client_base->set_free) {
        client_base->force_close(client_base);
        client_base->free_params(client_base);
    }
    
    zxy_add_block_to_link_list(proxy_conn->backend_handler_ptr);
    zxy_add_block_to_link_list(proxy_conn->client_handler_ptr);
}

void zxy_proxy_events_callback(void* handler, int sock_fd, u_int32_t events)
{
    zxy_event_handler_t *event_handler = (zxy_event_handler_t*)handler;
    zxy_backend_base_t *backend_base = get_backend_base_from(handler);
    zxy_client_base_t *client_base = get_client_base_from(handler);
    zxy_proxy_connection_t *proxy_conn = get_proxy_conn_from(handler);

    if (proxy_conn->timer_status_and_fd == TIMER_IS_UP)
        return;

    switch (event_handler->sock_type)
    {
    case CLIENT_SOCK: {
        zxy_handle_client_events(
            proxy_conn,
            backend_base,
            client_base,
            events
        );
        proxy_conn->timer_status_and_fd = RELAX_TIMER;
        break;
    }

    case BACKEND_SOCK: {
        zxy_handle_backend_events(
            proxy_conn,
            backend_base,
            client_base,
            events
        );
        proxy_conn->timer_status_and_fd = RELAX_TIMER;
        break;
    }

    case PROXY_TIMER_SOCK: {
        if (proxy_conn->timer_status_and_fd == RELAX_TIMER) {
            proxy_conn->timer_status_and_fd = TIMER_RELAXED;
            zxy_set_timer_time_with(sock_fd, 1, 0);
        } else if ( (proxy_conn->timer_status_and_fd == TIMER_RELAXED) ||
                (proxy_conn->timer_status_and_fd > 0) )
        {
            proxy_conn->timer_status_and_fd = TIMER_IS_UP;
            zxy_handle_timer_up_events(
                proxy_conn,
                backend_base,
                client_base
            );
            zxy_free_timer_handler(event_handler);
        }
        break;
    }
    
    default:
        break;
    }
}

void zxy_free_proxy_obj(void* params)
{
    zxy_proxy_connection_t *proxy_obj = (zxy_proxy_connection_t*)params;
    proxy_obj->refrence_counter--;

    if (proxy_obj->refrence_counter != 0)
        return;

    free(proxy_obj->client);
    free(proxy_obj->backend);
    
    free(proxy_obj);
}