


#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>

#include "defines.h"
#include "events/peers/backend_callbacks.h"
#include "logging/logs.h"
#include "utils/io/io_helper.h"
#include "utils/timer/timers.h"
void on_backend_read_event(void *ptr)
{
    backend_connection_info_t *backend_info = &( ((proxy_handler_t*)ptr)->backend_info);

    int nbytes;

    if (backend_info->read_buf[backend_info->max_bufer_size - 1] != '\0')
    {
        backend_info->read_buf = (char *)realloc(backend_info->read_buf, ((backend_info->max_bufer_size - 1) * 2 + 1) );
        memset((backend_info->read_buf + backend_info->max_bufer_size), '\0', (backend_info->max_bufer_size - 1));
        backend_info->max_bufer_size = ((backend_info->max_bufer_size - 1) * 2 + 1);
    }

    read_io_req_t read_req;

    read_req.req_fd = backend_info->backend_sock_fd;
    read_req.buffer = backend_info->read_buf + backend_info->buffer_ptr;
    read_req.maximum_read_buffer_size = READ_BUF_SIZE;
    read_req.flags = 0;

    nbytes = read_socket_non_block(&read_req);

    if (nbytes == 0) {
        LOG_WARNING("we should going to close\n");
        on_backend_close_event(ptr);
    } else if (nbytes == WOULD_BLOCK) {
        return;
    } else if (nbytes == UNKOWN_ERROR) {
        LOG_ERROR("fuck in recv\n");
    }

    LOG_INFO("number of readed bytes is %d(back)\n", nbytes);

    backend_info->buffer_ptr += nbytes;
    

    if (backend_info->max_bufer_size - 2 == nbytes)
    {
        backend_info->read_buf[backend_info->max_bufer_size - 1] = '1';
    }

    client_connection_info_t *client_info = &( ((proxy_handler_t*)ptr)->client_info);

    if (!client_info->is_client_closed && is_writable_event(client_info->client_events)) {
        client_info->client_handlers->on_write(ptr);
    }
}

void on_backend_write_event(void *ptr)
{
    client_connection_info_t *client_info = &( ((proxy_handler_t*)ptr)->client_info);
    if (client_info->buffer_ptr == 0)
        return;

    backend_connection_info_t *backend_info = &( ((proxy_handler_t*)ptr)->backend_info);

    int nbytes;
    write_io_req_t write_req;
        
    write_req.req_fd = backend_info->backend_sock_fd;
    write_req.buffer = client_info->read_buf;
    write_req.send_nbytes = client_info->buffer_ptr;
    write_req.clear_nbytes = client_info->max_bufer_size;
    write_req.flags = 0;

    nbytes = write_socket_non_block_and_clear_buf(&write_req);

    if (nbytes == 0) {
        LOG_WARNING("we should going to close\n");
        on_backend_close_event(ptr);
    } else if (nbytes == WOULD_BLOCK) {
        return;
    } else if (nbytes == UNKOWN_ERROR) {
        LOG_ERROR("fuck in send\n");
    }

    LOG_INFO("number of written bytes is %d(in send back)\n", nbytes);
    client_info->buffer_ptr = 0;
}

void on_backend_close_event(void *ptr)
{
    backend_connection_info_t *backend_info = &( ((proxy_handler_t*)ptr)->backend_info);

    remove_fd_from_epoll(backend_info->backend_sock_fd);
    LOG_INFO("backend fd(%d) closed the connection\n", backend_info->backend_sock_fd);
    backend_info->is_backend_closed = 1;
    close(backend_info->backend_sock_fd);

    add_block_to_link_list(((proxy_handler_t*)ptr)->backend_handler_ptr);
}


void backend_on_event_callback(int client_sock_fd, uint32_t events, void *ptr)
{
    proxy_handler_t *proxy_obj = (proxy_handler_t*)ptr;
    proxy_obj->backend_info.backend_events = events;

    if (proxy_obj->backend_info.is_backend_closed || proxy_obj->timer_status_and_fd == TIMER_IS_UP)
        return;


    if ((events & EPOLLHUP) | (events & EPOLLERR)) {//Error or close
        proxy_obj->backend_info.backend_handlers->on_close(ptr);
        return;
    }

    if (events & EPOLLIN) { //Read ready
        proxy_obj->backend_info.backend_handlers->on_read(ptr);
    }

    if (events & EPOLLOUT) { //Write ready
        proxy_obj->backend_info.backend_handlers->on_write(ptr);
    }
}