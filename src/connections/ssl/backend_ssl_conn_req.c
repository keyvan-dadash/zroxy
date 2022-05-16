#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "defines.h"
#include "logging/logs.h"
#include "events/io/epoll_manager.h"
#include "connections/conntypes/proxy_types.h"
#include "connections/conntypes/backend_types.h"
#include "connections/ssl/backend_ssl_conn_req.h"
#include "events/peers/ssl/ssl_backend_callbacks.h"

zxy_backend_base_t* zxy_make_ssl_backend_base_conn(void *params)
{
    zxy_backend_base_t *backend_base_conn = (zxy_backend_base_t*)calloc(1, sizeof(zxy_backend_base_t));

    backend_base_conn->params = params;
    backend_base_conn->set_free = 0;

    return backend_base_conn;
}

zxy_backend_ssl_conn_t* zxy_make_backend_ssl_conn(SSL_CTX *ctx, int sock_fd)
{
    zxy_backend_ssl_conn_t *backend_conn = (zxy_backend_ssl_conn_t*)calloc(1, sizeof(zxy_backend_ssl_conn_t));

    backend_conn->sock_fd = sock_fd;
    backend_conn->is_closed = 0;
    backend_conn->set_free = 0;
    backend_conn->encrypt_buffer_manager = zxy_malloc_buffer_manager_with_buffer_size(READ_BUF_SIZE + 1);
    backend_conn->writing_buffer_manager = zxy_malloc_buffer_manager_with_buffer_size(READ_BUF_SIZE + 1);
    backend_conn->read_buffer_manager = zxy_malloc_buffer_manager_with_buffer_size(READ_BUF_SIZE + 1);
    backend_conn->plain_buffer_manager = zxy_malloc_buffer_manager_with_buffer_size(READ_BUF_SIZE + 1);
    backend_conn->rbio = BIO_new(BIO_s_mem());
    backend_conn->wbio = BIO_new(BIO_s_mem());
    backend_conn->ssl = SSL_new(ctx);
    
    SSL_set_connect_state(backend_conn->ssl);
    SSL_set_bio(backend_conn->ssl, backend_conn->rbio, backend_conn->wbio);

    return backend_conn;
}

void zxy_set_up_backend_ssl_base_callbacks(zxy_backend_base_t* backend_base)
{
    backend_base->free_params = zxy_free_backend_ssl;
    backend_base->force_close = zxy_backend_ssl_force_close;
    backend_base->is_ready_event = zxy_backend_ssl_is_ready_for_event;
    backend_base->on_close = zxy_on_backend_ssl_close_event;
    backend_base->on_read = zxy_on_backend_ssl_read_event;
    backend_base->on_write = zxy_on_backend_ssl_write_event;
    backend_base->request_buffer_reader = zxy_backend_ssl_request_buffer_reader;
    backend_base->read_nbytes = zxy_backend_ssl_read_nbytes_from_buffer;
}

