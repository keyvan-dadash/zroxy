


#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>

#include "defines.h"
#include "connections/conntypes/backend_types.h"
#include "events/peers/plain/backend_callbacks.h"
#include "logging/logs.h"
#include "utils/io/io_helper.h"
#include "utils/io/buffer_manager.h"
#include "utils/timer/timers.h"


zxy_backend_conn_t* convert_backend_conn(void *ptr)
{
    return (zxy_backend_conn_t*)(((zxy_backend_base_t*)ptr)->params);
}

zxy_backend_base_t* convert_backend_base(void *ptr)
{
    return (zxy_backend_base_t*)ptr;
}


int zxy_on_backend_plain_read_event(void *ptr)
{
    zxy_backend_conn_t *backend_conn = convert_backend_conn(ptr);

    int nbytes;

    if (zxy_should_resize_buffer(backend_conn->buffer_manager))
    {
        zxy_double_buffer_size(backend_conn->buffer_manager);
    }

    zxy_read_io_req_t read_req;

    read_req.req_fd = backend_conn->sock_fd;
    read_req.buffer = backend_conn->buffer_manager->buffer + backend_conn->buffer_manager->current_buffer_ptr;
    read_req.maximum_read_buffer_size = READ_BUF_SIZE;
    read_req.flags = 0;

    nbytes = zxy_read_socket_non_block(&read_req);

    if (nbytes == 0) {
        LOG_WARNING("Read 0 bytes from FD(%d) and we should close this fd\n", backend_conn->sock_fd);
        return 0; //TODO: some how signal close state
    } else if (nbytes == WOULD_BLOCK) {
        return WOULD_BLOCK;
    } else if (nbytes == UNKOWN_ERROR) {
        LOG_ERROR("We encounter unkown error for FD(%d)\n", backend_conn->sock_fd);
        return UNKOWN_ERROR;
    }


    zxy_nbyte_written_to_buffer(backend_conn->buffer_manager, nbytes);

    return nbytes;
}

int zxy_on_backend_plain_write_event(void *ptr, zxy_write_io_req_t* write_req)
{
    zxy_backend_conn_t *backend_conn = convert_backend_conn(ptr);

    int nbytes;
        
    write_req->req_fd = backend_conn->sock_fd;

    // TODO: some how it closed
    // write_req->flags |= MSG_NOSIGNAL;

    nbytes = zxy_write_socket_non_block_and_clear_buf(write_req);

    if (nbytes == 0) {
        LOG_WARNING("Write 0 bytes to FD(%d) and we should close this fd\n", backend_conn->sock_fd);
        return 0;
    } else if (nbytes == WOULD_BLOCK) {
        return WOULD_BLOCK;
    } else if (nbytes == UNKOWN_ERROR) {
        LOG_ERROR("We encounter unkown error for FD(%d)\n", backend_conn->sock_fd);
        return UNKOWN_ERROR;
    }

    LOG_INFO("Number of written bytes is %d\n", nbytes);

    return nbytes;
}

int zxy_on_backend_plain_close_event(void *ptr)
{
    zxy_backend_conn_t *backend_conn = convert_backend_conn(ptr);

    zxy_remove_fd_from_epoll(backend_conn->sock_fd);
    LOG_INFO("Backend FD(%d) closed the connection\n", backend_conn->sock_fd);
    backend_conn->is_closed = 1;
    close(backend_conn->sock_fd);

    return 1;
}


zxy_write_io_req_t zxy_backend_plain_request_buffer_reader(void *ptr)
{
    zxy_backend_conn_t *backend_conn = convert_backend_conn(ptr);

    zxy_write_io_req_t write_req;
    write_req.buffer = backend_conn->buffer_manager->buffer;
    write_req.flags = 0;
    write_req.send_nbytes = backend_conn->buffer_manager->current_buffer_ptr;
    write_req.clear_nbytes = backend_conn->buffer_manager->current_buffer_ptr;
    
    return write_req;
}


//TODO: change logic of force close
int zxy_backend_plain_force_close(void *ptr)
{
    zxy_backend_conn_t *backend_conn = convert_backend_conn(ptr);

    if (backend_conn->is_closed != 1) {
        zxy_remove_fd_from_epoll(backend_conn->sock_fd);
        LOG_INFO("Backend fd(%d) closed the connection\n", backend_conn->sock_fd);
        backend_conn->is_closed = 1;
        close(backend_conn->sock_fd);

        return 1;
    }
    return 0;
}

int zxy_backend_plain_is_ready_for_event(u_int32_t events, u_int32_t is_ready, void* ptr)
{
    zxy_backend_conn_t *backend_conn = convert_backend_conn(ptr);

    if (events != -1) {
        backend_conn->events = events;
    }

    //TODO: we can handle better
    switch (is_ready)
    {
    case READ_EVENT: {
        if (backend_conn->events & EPOLLIN) return 1;
        return 0;
        break;
    }
    case WRITE_EVENT: {
        if (backend_conn->events & EPOLLOUT) return 1;
        return 0;
        break;
    }
    case CLOSE_EVENT: {
        if ((backend_conn->events & EPOLLHUP) | (backend_conn->events & EPOLLERR)) return 1;
        return 0;
        break;
    }
    
    default:
        return 0;
    }
}

void zxy_free_backend_plain(void *ptr)
{
    zxy_backend_base_t *backend_base = convert_backend_base(ptr);
    zxy_backend_conn_t *backend_conn = convert_backend_conn(ptr);

    zxy_free_buffer_manager(backend_conn->buffer_manager);
    
    free(backend_conn);

    backend_base->set_free = 1;
}
