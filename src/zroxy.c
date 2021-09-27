
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "defines.h"
#include "handle_client.h"
#include "netutils.h"
#include "server.h"

int main(int argc, char *argv[])
{
    char *server_port_char;
    char *backend_address;
    char *backend_port_char;


    struct addrinfo hints;
    struct addrinfo *addrs;
    struct addrinfo *addrs_iter;
    int getaddrinfo_error;

    int server_sock;

    int so_reuseaddr;

    if (argc != 4) {
        fprintf(stderr,
            "Usage: %s, <server_port> <backend_addr> <backend_port>\n",
            argv[0]);
        exit(-1);
    }


    server_port_char = argv[1];
    backend_address = argv[2];
    backend_port_char = argv[3];

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_protocol = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo_error = getaddrinfo(backend_address, backend_port_char, &hints, &addrs);

    if (getaddrinfo_error != 0) {
        fprintf(stderr, "cannot connect to backend: %s\n", gai_strerror(getaddrinfo_error));
        exit(-1);
    } 

    for (addrs_iter = addrs; 
         addrs_iter != NULL; 
         addrs_iter = addrs_iter->ai_next) {

        server_sock = socket(addrs_iter->ai_family, addrs_iter->ai_socktype, addrs_iter->ai_protocol);

        if (server_sock == -1) 
            continue;

        so_reuseaddr = 1;
        setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));

        if (bind(server_sock, addrs_iter->ai_addr, addrs_iter->ai_addrlen))
            break;

        close(server_sock);
    }

    if (addrs_iter == NULL) {
        fprintf(stderr, "cannot setup server");
        exit(-1);
    }

    freeaddrinfo(addrs);

    if (listen(server_sock, MAX_LISTEN_SIZE) == -1) {
        fprintf(stderr, "cannot listen");
        exit(-1);
    }

    fprintf(stdout, "Listen on port: %s", server_port_char);



    backend_addrs_t backend_addrs;

    backend_addrs.backend_host = backend_address;
    backend_addrs.backend_port = backend_port_char;

    make_socket_nonblock(server_sock);

    start_server(server_sock, &backend_addrs);
}