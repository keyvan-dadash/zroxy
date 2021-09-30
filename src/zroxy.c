
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include <openssl/ssl.h>

#include "defines.h"
#include "handle_client.h"
#include "netutils.h"
#include "server.h"
#include "conf/configure.h"
#include "ssl_helper.h"


int main(int argc, char *argv[])
{

    struct addrinfo hints;
    struct addrinfo *addrs;
    struct addrinfo *addrs_iter;
    int getaddrinfo_error;

    int server_sock;

    int so_reuseaddr;

    if (argc != 2) {
        fprintf(stderr,
            "Usage: %s, <config_file>\n",
            argv[0]);
        exit(-1);
    }

    connection_configs_t config = get_configs_from_file(argv[1]);
    SSL_CTX *ctx = ssl_init();
    ssl_load_certificates_and_private_keys(ctx, config.certificate_path, config.private_key_path);

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo_error = getaddrinfo(NULL, config.server_port_char, &hints, &addrs);

    if (getaddrinfo_error != 0) {
        fprintf(stderr, "cannot find backend: %s\n", gai_strerror(getaddrinfo_error));
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

        if (bind(server_sock, addrs_iter->ai_addr, addrs_iter->ai_addrlen) == 0)
            break;

        close(server_sock);
    }

    if (addrs_iter == NULL) {
        fprintf(stderr, "cannot setup server proper reason %s\n", strerror(errno));
        exit(-1);
    }

    freeaddrinfo(addrs);

    if (listen(server_sock, MAX_LISTEN_SIZE) == -1) {
        fprintf(stderr, "cannot listen\n");
        exit(-1);
    }

    fprintf(stdout, "Listen on port: %s\n", config.server_port_char);


    epoll_init();

    backend_addrs_t backend_addrs;

    backend_addrs.backend_host = config.backend_address;
    backend_addrs.backend_port = config.backend_port_char;

    make_socket_nonblock(server_sock);

    start_server(server_sock, &backend_addrs);
}