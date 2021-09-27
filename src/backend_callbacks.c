




#include "backend_callbacks.h"


void on_backend_read_event(proxy_handler_t *proxy_obj)
{
    client_connection_info_t *client_info = &(proxy_obj->client_info);
    backend_connection_info_t *backend_info = &(proxy_obj->backend_info);

    int nbytes;
    int error;

    if (backend_info->read_buf[backend_info->max_bufer_size - 1] != '\0')
    {
        backend_info->read_buf = realloc(backend_info->read_buf, backend_info->max_bufer_size * 2);
        memset((backend_info->read_buf + backend_info->max_bufer_size), '\0', backend_info->max_bufer_size);
        backend_info->max_bufer_size *= 2;
    }


    

    nbytes = recv(backend_info->backend_sock_fd, 
                           backend_info->read_buf, 
                           backend_info->max_bufer_size - 1, 0);
    

    if (backend_info->max_bufer_size - 2 == nbytes)
    {
        backend_info->read_buf[backend_info->max_bufer_size - 1] == '1';
    }
}

void on_backend_write_event(proxy_handler_t *proxy_obj)
{
    client_connection_info_t *client_info = &(proxy_obj->client_info);
    backend_connection_info_t *backend_info = &(proxy_obj->backend_info);

    int nbytes;
    int error;


    nbytes = send(backend_info->backend_sock_fd, 
                           client_info->read_buf, 
                           client_info->max_bufer_size - 1, 0);

    memset(client_info->read_buf, '\0', client_info->max_bufer_size);
}

void on_backend_close_event(proxy_handler_t *proxy_obj)
{
    backend_connection_info_t *backend_info = &(proxy_obj->backend_info);
    remove_fd_from_epoll(backend_info->backend_sock_fd);
    backend_info->is_backend_closed = 1;
    close(backend_info->backend_sock_fd);
}


void backend_on_event_callback(int client_sock_fd, uint32_t events, void *ptr)
{
    proxy_handler_t *proxy_obj = (proxy_handler_t*)ptr;


    if (events & (EPOLLHUP || EPOLLERR)) {//Error or close
        proxy_obj->backend_info.backend_handlers->on_close(proxy_obj);
        return;
    }

    if (events & EPOLLIN) { //Write ready
        proxy_obj->backend_info.backend_handlers->on_write(proxy_obj);
    }

    if (events & EPOLLOUT) { //Read ready
        proxy_obj->backend_info.backend_handlers->on_read(proxy_obj);
    }
}