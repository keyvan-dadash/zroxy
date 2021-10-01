



#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>

#include "defines.h"

#include "connections/conntypes/client_types.h"
#include "events/peers/client_callbacks.h"
#include "logging/logs.h"
#include "utils/io/io_helper.h"
#include "utils/io/buffer_mamager.h"
#include "utils/timer/timers.h"


int zxy_on_client_plain_read_event(void *ptr)
{
    zxy_client_base_t *client_base = (zxy_client_base_t*)ptr;
    zxy_client_conn_t *client_conn = (zxy_client_conn_t*)(client_base->params);

    int nbytes;

    if (zxy_should_resize_buffer(client_conn->buffer_manager)) {
        zxy_double_buffer_size(client_conn->buffer_manager);
    }

    zxy_read_io_req_t read_req;

    read_req.req_fd = client_conn->sock_fd;
    read_req.buffer = client_conn->buffer_manager->buffer + client_conn->buffer_manager->current_buffer_ptr;
    read_req.maximum_read_buffer_size = READ_BUF_SIZE;
    read_req.flags = 0;

    nbytes = zxy_read_socket_non_block(&read_req);

    if (nbytes == 0) {
        LOG_WARNING("we should going to close\n");
        return 0; //TODO: some how signal close state
    } else if (nbytes == WOULD_BLOCK) {
        return WOULD_BLOCK;
    } else if (nbytes == UNKOWN_ERROR) {
        LOG_ERROR("fuck in recv\n");
        return UNKOWN_ERROR;
    }

    zxy_nbyte_written_to_buffer(client_conn->buffer_manager, nbytes);




    // if (client_info->read_buf[client_info->max_bufer_size - 1] != '\0')
    // {
    //     client_info->read_buf = (char *)realloc(client_info->read_buf, ((client_info->max_bufer_size - 1) * 2 + 1) );
    //     memset((client_info->read_buf + client_info->max_bufer_size), '\0', (client_info->max_bufer_size - 1));
    //     client_info->max_bufer_size = ((client_info->max_bufer_size - 1) * 2 + 1);
    // }

    // read_io_req_t read_req;

    // read_req.req_fd = client_info->client_sock_fd;
    // read_req.buffer = client_info->read_buf + client_info->buffer_ptr;
    // read_req.maximum_read_buffer_size = READ_BUF_SIZE;
    // read_req.flags = 0;

    // nbytes = read_socket_non_block(&read_req);

    // if (nbytes == 0) {
    //     LOG_WARNING("we should going to close\n");
    //     on_client_close_event(ptr);
    // } else if (nbytes == WOULD_BLOCK) {
    //     return;
    // } else if (nbytes == UNKOWN_ERROR) {
    //     LOG_ERROR("fuck in recv\n");
    // }

    // client_info->buffer_ptr += nbytes;
    

    // if (client_info->max_bufer_size - 2 == nbytes)
    // {
    //     client_info->read_buf[client_info->max_bufer_size - 1] = '1';
    // }

    // backend_connection_info_t *backend_info = &( ((proxy_handler_t*)ptr)->backend_info);

    // if (!backend_info->is_backend_closed && is_writable_event(backend_info->backend_events)) {
    //     backend_info->backend_handlers->on_write(ptr);
    // }
}

int zxy_on_client_plain_write_event(void *ptr)
{
    zxy_client_base_t *client_base = (zxy_client_base_t*)ptr;
    zxy_client_conn_t *client_conn = (zxy_client_conn_t*)(client_base->params);

    int nbytes;
    zxy_write_io_req_t write_req;
        
    write_req.req_fd = client_conn->sock_fd;
    write_req.buffer = client_conn->buffer_manager->buffer;
    write_req.send_nbytes = backend_info->buffer_ptr;
    write_req.clear_nbytes = backend_info->max_bufer_size;
    write_req.flags = 0;

    nbytes = write_socket_non_block_and_clear_buf(&write_req);

    if (nbytes == 0) {
        LOG_WARNING("we should going to close\n");
        on_client_close_event(ptr);
    } else if (nbytes == WOULD_BLOCK) {
        return;
    } else if (nbytes == UNKOWN_ERROR) {
        LOG_ERROR("fuck in send\n");
    }

    LOG_INFO("number of written bytes is %d(in send client)\n", nbytes);
    backend_info->buffer_ptr = 0;
}

int zxy_on_client_plain_close_event(void *ptr)
{
    client_connection_info_t *client_info = &( ((proxy_handler_t*)ptr)->client_info);

    remove_fd_from_epoll(client_info->client_sock_fd);
    LOG_INFO("Client fd(%d) closed the connection\n", client_info->client_sock_fd);
    client_info->is_client_closed = 1;
    close(client_info->client_sock_fd);

    add_block_to_link_list(((proxy_handler_t*)ptr)->client_handler_ptr);
}



void zxy_client_on_event_callback(int client_sock_fd, uint32_t events, void *ptr)
{
    proxy_handler_t *proxy_obj = (proxy_handler_t*)ptr;
    proxy_obj->client_info.client_events = events;

    if (proxy_obj->client_info.is_client_closed || proxy_obj->timer_status_and_fd == TIMER_IS_UP)
        return;


    if ((events & EPOLLHUP) | (events & EPOLLERR)) {//Error or close
        proxy_obj->client_info.client_handlers->on_close(ptr);
        return;
    }

    if (is_readable_event(events)) { //Read ready
        proxy_obj->client_info.client_handlers->on_read(ptr);
    }

    if (is_writable_event(events)) { //Write ready
        proxy_obj->client_info.client_handlers->on_write(ptr);
    }
}