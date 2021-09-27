




#include "backend_callbacks.h"


void on_backend_read_event(backend_connection_info_t *backend_info, client_connection_info_t *client_info)
{
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

void on_backend_write_event(backend_connection_info_t *backend_info, client_connection_info_t *client_info)
{
    int nbytes;
    int error;


    nbytes = send(backend_info->backend_sock_fd, 
                           client_info->read_buf, 
                           client_info->max_bufer_size - 1, 0);

    memset(client_info->read_buf, '\0', client_info->max_bufer_size);
}

void on_backend_close_event(backend_connection_info_t *backend_info)
{
    remove_fd_from_epoll(backend_info->backend_sock_fd);
    backend_info->is_backend_closed = 1;
    close(backend_info->backend_sock_fd);
}