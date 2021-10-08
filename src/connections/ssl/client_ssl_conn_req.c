


#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "defines.h"
#include "events/io/epoll_manager.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/client_types.h"
#include "connections/ssl/client_ssl_conn_req.h"


zxy_client_base_t* zxy_make_ssl_client_base_conn(void *params)
{
    zxy_client_base_t *client_base_conn = (zxy_client_base_t*)calloc(1, sizeof(zxy_client_base_t));

    client_base_conn->params = params;
    client_base_conn->set_free = 0;

    return client_base_conn;
}


zxy_client_ssl_conn_t* zxy_make_client_ssl_conn(SSL_CTX *ctx, int sock_fd)
{
    zxy_client_ssl_conn_t *client_conn = (zxy_client_ssl_conn_t*)calloc(1, sizeof(zxy_client_ssl_conn_t));

    client_conn->sock_fd = sock_fd;
    client_conn->is_closed = 0;
    client_conn->set_free = 0;
    client_conn->encrypt_buffer_manager = zxy_malloc_buffer_manager_with_buffer_size(READ_BUF_SIZE + 1);
    client_conn->writing_buffer_manager = zxy_malloc_buffer_manager_with_buffer_size(READ_BUF_SIZE + 1);
    client_conn->rbio = BIO_new(BIO_s_mem());
    client_conn->wbio = BIO_new(BIO_s_mem());
    client_conn->ssl = SSL_new(ctx);

    SSL_set_accept_state(client_conn->ssl);
    SSL_set_bio(client_conn->ssl, client_conn->rbio, client_conn->wbio);

    return client_conn;
}

void zxy_set_up_client_ssl_base_callbacks(zxy_client_ssl_conn_t* client_base)
{

}