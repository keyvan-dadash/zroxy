



#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>

#include "defines.h"
#include "client_callbacks.h"
#include "logs.h"


//TODO: some refactor on wrteing and reading

void on_client_read_event(void *ptr)
{
    client_connection_info_t *client_info = &( ((proxy_handler_t*)ptr)->client_info);

    int nbytes;

    if (client_info->read_buf[client_info->max_bufer_size - 1] != '\0')
    {
        client_info->read_buf = (char *)realloc(client_info->read_buf, ((client_info->max_bufer_size - 1) * 2 + 1) );
        memset((client_info->read_buf + client_info->max_bufer_size), '\0', (client_info->max_bufer_size - 1));
        client_info->max_bufer_size = ((client_info->max_bufer_size - 1) * 2 + 1);
    }


    nbytes = recv(client_info->client_sock_fd, 
                           client_info->read_buf, 
                           client_info->max_bufer_size - 1, 0);
    
    if (nbytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        return;
    else if (nbytes == -1)
        LOG_ERROR("Cannot read from client with fd(%d) becuase: %s\n", client_info->client_sock_fd, strerror(errno));

    LOG_INFO("number of readed bytes is %d(client)\n", nbytes);

    client_info->buffer_ptr = nbytes;
    

    if (client_info->max_bufer_size - 2 == nbytes)
    {
        client_info->read_buf[client_info->max_bufer_size - 1] = '1';
    }

    backend_connection_info_t *backend_info = &( ((proxy_handler_t*)ptr)->backend_info);

    if (backend_info->backend_events & EPOLLOUT) {
        nbytes = send(backend_info->backend_sock_fd, 
                    client_info->read_buf, 
                    client_info->buffer_ptr, 0);

        if (nbytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
            return;
        else if (nbytes == -1)
            LOG_ERROR("Cannot write to backend with client fd(%d) and backend fd(%d) becuase: %s\n", 
                        client_info->client_sock_fd, backend_info->backend_sock_fd, strerror(errno));

        LOG_INFO("number of written bytes is %d(in recv client)\n", nbytes);

        memset(client_info->read_buf, '\0', client_info->max_bufer_size);
        client_info->buffer_ptr = -1;
    }
}

void on_client_write_event(void *ptr)
{
    backend_connection_info_t *backend_info = &( ((proxy_handler_t*)ptr)->backend_info);
    if (backend_info->buffer_ptr == -1)
        return;

    client_connection_info_t *client_info = &( ((proxy_handler_t*)ptr)->client_info);

    int nbytes;

    nbytes = send(client_info->client_sock_fd, 
                backend_info->read_buf, 
                backend_info->buffer_ptr, 0);
    
    if (nbytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        return;
    else if (nbytes == -1)
        LOG_ERROR("Cannot write to client with client fd(%d) and backend fd(%d) becuase: %s\n", 
                    client_info->client_sock_fd, backend_info->backend_sock_fd, strerror(errno));

    LOG_INFO("number of written bytes is %d(in send client)\n", nbytes);

    memset(backend_info->read_buf, '\0', backend_info->max_bufer_size);
    backend_info->buffer_ptr = -1;

    // if (client_info->read_buf[client_info->max_bufer_size - 1] != '\0')
    // {
    //     client_info->read_buf = realloc(client_info->read_buf, client_info->max_bufer_size * 2);
    //     memset((client_info->read_buf + client_info->max_bufer_size), '\0', client_info->max_bufer_size);
    //     client_info->max_bufer_size *= 2;
    // }


    

    // nbytes = recv(client_info->client_sock_fd, 
    //                        client_info->read_buf, 
    //                        client_info->max_bufer_size - 1, 0);
    

    // if (client_info->max_bufer_size - 2 == nbytes)
    // {
    //     client_info->read_buf[client_info->max_bufer_size - 1] == '1';
    // }
}

void on_client_close_event(void *ptr)
{
    client_connection_info_t *client_info = &( ((proxy_handler_t*)ptr)->client_info);

    remove_fd_from_epoll(client_info->client_sock_fd);
    LOG_INFO("Client fd(%d) closed the connection\n", client_info->client_sock_fd);
    client_info->is_client_closed = 1;
    close(client_info->client_sock_fd);
}



void client_on_event_callback(int client_sock_fd, uint32_t events, void *ptr)
{
    proxy_handler_t *proxy_obj = (proxy_handler_t*)ptr;
    proxy_obj->client_info.client_events = events;


    if ((events & EPOLLHUP) | (events & EPOLLERR)) {//Error or close
        proxy_obj->client_info.client_handlers->on_close(ptr);
        return;
    }

    if (events & EPOLLIN) { //Read ready
        proxy_obj->client_info.client_handlers->on_read(ptr);
    }

    if (events & EPOLLOUT) { //Write ready
        proxy_obj->client_info.client_handlers->on_write(ptr);
    }
}