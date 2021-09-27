



#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <memory.h>


#include "defines.h"
#include "client_callbacks.h"


void on_client_read_event(client_connection_info_t *client_info, backend_connection_info_t *backend_info)
{
    int nbytes;
    int error;

    if (client_info->read_buf[client_info->max_bufer_size - 1] != '\0')
    {
        client_info->read_buf = realloc(client_info->read_buf, client_info->max_bufer_size * 2);
        memset((client_info->read_buf + client_info->max_bufer_size), '\0', client_info->max_bufer_size);
        client_info->max_bufer_size *= 2;
    }


    

    nbytes = recv(client_info->client_sock_fd, 
                           client_info->read_buf, 
                           client_info->max_bufer_size - 1, 0);
    

    if (client_info->max_bufer_size - 2 == nbytes)
    {
        client_info->read_buf[client_info->max_bufer_size - 1] == '1';
    }
}

void on_client_write_event(client_connection_info_t *client_info, backend_connection_info_t *backend_info)
{
    int nbytes;
    int error;


    nbytes = send(client_info->client_sock_fd, 
                           backend_info->read_buf, 
                           backend_info->max_bufer_size - 1, 0);

    memset(backend_info->read_buf, '\0', backend_info->max_bufer_size);

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

void on_client_close_event(client_connection_info_t *client_info)
{
    remove_fd_from_epoll(client_info->client_sock_fd);
    client_info->is_client_closed = 1;
    close(client_info->client_sock_fd);
}