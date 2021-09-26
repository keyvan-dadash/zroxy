


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#include "defines.h"

int handle_client_connection(int client_sock, char *backend_host, char *backend_port)
{

    struct addrinfo hints;
    struct addrinfo *addrs;
    struct addrinfo *addrs_iter;
    int getaddrinfo_error;

    int backend_socket_fd;

    char buf[READ_BUF_SIZE];
    int nbytes;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_protocol = SOCK_STREAM;

    getaddrinfo_error = getaddrinfo(backend_host, backend_port, &hints, &addrs);

    if (getaddrinfo != 0) {
        fprintf(stderr, "cannot connect to backend: %s\n", gai_strerror(getaddrinfo_error));
        exit(-1);
    } 

    for (addrs_iter = addrs; 
         addrs_iter != NULL; 
         addrs_iter = addrs_iter->ai_next) {

        backend_socket_fd = socket(addrs_iter->ai_family, addrs_iter->ai_socktype, addrs_iter->ai_protocol);

        if (backend_socket_fd == -1) 
            continue;

        if (connect(backend_socket_fd, addrs_iter->ai_addr, addrs_iter->ai_addrlen) != -1)
            break;

        close(backend_socket_fd);
    }

    if (addrs_iter == NULL) {
        fprintf(stderr, "cannot connect to any backend");
    }

    freeaddrinfo(addrs);

    while (nbytes = recv(client_sock, buf, READ_BUF_SIZE, 0)) {
        send(client_sock, buf, READ_BUF_SIZE, 0);
    }

    close(backend_socket_fd);

}
