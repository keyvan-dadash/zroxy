



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
#include "events/peers/plain/client_callbacks.h"
#include "logging/logs.h"
#include "utils/io/io_helper.h"
#include "utils/io/buffer_manager.h"
#include "utils/timer/timers.h"

zxy_client_conn_t* convert_client_conn(void *ptr)
{
    return (zxy_client_conn_t*)(((zxy_client_base_t*)ptr)->params);
}

zxy_client_base_t* convert_client_base(void *ptr)
{
    return (zxy_client_base_t*)ptr;
}

int zxy_on_client_plain_read_event(void *ptr)
{
    zxy_client_conn_t *client_conn = convert_client_conn(ptr);

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
        LOG_WARNING("we should going to close(plain client)\n");
        return 0; //TODO: some how signal close state
    } else if (nbytes == WOULD_BLOCK) {
        return WOULD_BLOCK;
    } else if (nbytes == UNKOWN_ERROR) {
        LOG_ERROR("fuck in recv\n");
        return UNKOWN_ERROR;
    }

    zxy_nbyte_written_to_buffer(client_conn->buffer_manager, nbytes);

    return nbytes;
}

int zxy_on_client_plain_write_event(void *ptr, zxy_write_io_req_t* write_req)
{
    zxy_client_conn_t *client_conn = convert_client_conn(ptr);

    int nbytes;
        
    write_req->req_fd = client_conn->sock_fd;

    nbytes = zxy_write_socket_non_block_and_clear_buf(write_req);

    if (nbytes == 0) {
        LOG_WARNING("we should going to close(palin client)\n");
        return 0;
    } else if (nbytes == WOULD_BLOCK) {
        return WOULD_BLOCK;
    } else if (nbytes == UNKOWN_ERROR) {
        LOG_ERROR("fuck in send\n");
        return UNKOWN_ERROR;
    }

    LOG_INFO("number of written bytes is %d(in send client)\n", nbytes);
    
    return nbytes;
}

int zxy_on_client_plain_close_event(void *ptr)
{
    zxy_client_conn_t *client_conn = convert_client_conn(ptr);

    zxy_remove_fd_from_epoll(client_conn->sock_fd);
    LOG_INFO("client fd(%d) closed the connection\n", client_conn->sock_fd);
    client_conn->is_closed = 1;
    close(client_conn->sock_fd);

    return 1;
}


zxy_write_io_req_t zxy_client_plain_request_buffer_reader(void *ptr)
{
    zxy_client_conn_t *client_conn = convert_client_conn(ptr);

    zxy_write_io_req_t write_req;
    write_req.buffer = client_conn->buffer_manager->buffer;
    write_req.flags = 0;
    write_req.send_nbytes = client_conn->buffer_manager->current_buffer_ptr;
    write_req.clear_nbytes = client_conn->buffer_manager->current_buffer_ptr;
    
    return write_req;
}


//TODO: change logic of force close
int zxy_client_plain_force_close(void *ptr)
{
    zxy_client_conn_t *client_conn = convert_client_conn(ptr);

    if (client_conn->is_closed != 1) {
        zxy_remove_fd_from_epoll(client_conn->sock_fd);
        LOG_INFO("backend fd(%d) closed the connection\n", client_conn->sock_fd);
        client_conn->is_closed = 1;
        close(client_conn->sock_fd);

        return 1;
    }

    return 0;
}

int zxy_client_plain_is_ready_for_event(u_int32_t events, u_int32_t is_ready, void* ptr)
{
    zxy_client_conn_t *client_conn = convert_client_conn(ptr);

    if (events != -1) {
        client_conn->events = events;
    }

    switch (is_ready)
    {
    case READ_EVENT: {
        if (client_conn->events & EPOLLIN) return 1;
        return 0;
        break;
    }
    case WRITE_EVENT: {
        if (client_conn->events & EPOLLOUT) return 1;
        return 0;
        break;
    }
    case CLOSE_EVENT: {
        if ((client_conn->events & EPOLLHUP) | (client_conn->events & EPOLLERR)) return 1;
        return 0;
        break;
    }
    
    default:
        return 0;
    }
}

void zxy_free_client_plain(void *ptr)
{
    zxy_client_base_t *client_base = convert_client_base(ptr);
    zxy_client_conn_t *client_conn = convert_client_conn(ptr);

    zxy_free_buffer_manager(client_conn->buffer_manager);
    
    free(client_conn);

    client_base->set_free = 1;
}