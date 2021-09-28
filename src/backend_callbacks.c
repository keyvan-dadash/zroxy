


#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>

#include "defines.h"
#include "backend_callbacks.h"
#include "logs.h"

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


    
    LOG_INFO("block1\n");

    nbytes = recv(backend_info->backend_sock_fd, 
                           backend_info->read_buf, 
                           backend_info->max_bufer_size - 1, 0);

    if (nbytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        return;
    else if (nbytes == -1)
        LOG_ERROR("cannot read from fd(%d) becuase: %s\n", backend_info->backend_sock_fd, strerror(errno));

    LOG_INFO("number of readed bytes is %d(back)\n", nbytes);

    backend_info->buffer_ptr = nbytes;
    

    if (backend_info->max_bufer_size - 2 == nbytes)
    {
        backend_info->read_buf[backend_info->max_bufer_size - 1] = '1';
    }

    client_connection_info_t *client_info = &( ((proxy_handler_t*)ptr)->client_info);
    if (client_info->client_events & EPOLLOUT) {
        nbytes = send(client_info->client_sock_fd, 
                backend_info->read_buf, 
                backend_info->buffer_ptr, 0);
    
        if (nbytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
            return;
        else if (nbytes == -1)
            LOG_ERROR("Cannot write to client with client fd(%d) and backend fd(%d) becuase: %s\n", 
                        client_info->client_sock_fd, backend_info->backend_sock_fd, strerror(errno));

        LOG_INFO("number of written bytes is %d(in recv back)\n", nbytes);

        memset(backend_info->read_buf, '\0', backend_info->max_bufer_size);
        backend_info->buffer_ptr = -1;
    }
}

void on_backend_write_event(void *ptr)
{
    client_connection_info_t *client_info = &( ((proxy_handler_t*)ptr)->client_info);
    if (client_info->buffer_ptr == -1)
        return;

    backend_connection_info_t *backend_info = &( ((proxy_handler_t*)ptr)->backend_info);

    int nbytes;

    nbytes = send(backend_info->backend_sock_fd, 
                client_info->read_buf, 
                client_info->buffer_ptr, 0);

    if (nbytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        return;
    else if (nbytes == -1)
        LOG_ERROR("Cannot write to backend with client fd(%d) and backend fd(%d) becuase: %s\n", 
                    client_info->client_sock_fd, backend_info->backend_sock_fd, strerror(errno));

    LOG_INFO("number of written bytes is %d(in send back)\n", nbytes);

    memset(client_info->read_buf, '\0', client_info->max_bufer_size);
    client_info->buffer_ptr = -1;
}

void on_backend_close_event(void *ptr)
{
    backend_connection_info_t *backend_info = &( ((proxy_handler_t*)ptr)->backend_info);

    remove_fd_from_epoll(backend_info->backend_sock_fd);
    LOG_INFO("backend fd(%d) closed the connection\n", backend_info->backend_sock_fd);
    backend_info->is_backend_closed = 1;
    close(backend_info->backend_sock_fd);
}


void backend_on_event_callback(int client_sock_fd, uint32_t events, void *ptr)
{
    proxy_handler_t *proxy_obj = (proxy_handler_t*)ptr;
    proxy_obj->backend_info.backend_events = events;


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