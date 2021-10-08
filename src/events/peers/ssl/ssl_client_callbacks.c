



#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>

#include "defines.h"

#include "connections/conntypes/client_types.h"
#include "events/peers/ssl/ssl_client_callbacks.h"
#include "logging/logs.h"
#include "utils/io/io_helper.h"
#include "utils/io/buffer_manager.h"
#include "utils/timer/timers.h"


enum sslstatus { SSLSTATUS_OK, SSLSTATUS_WANT_IO, SSLSTATUS_FAIL};

static enum sslstatus get_sslstatus(SSL* ssl, int n)
{
  switch (SSL_get_error(ssl, n))
  {
    case SSL_ERROR_NONE:
      return SSLSTATUS_OK;
    case SSL_ERROR_WANT_WRITE:
    case SSL_ERROR_WANT_READ:
      return SSLSTATUS_WANT_IO;
    case SSL_ERROR_ZERO_RETURN:
    case SSL_ERROR_SYSCALL:
    default:
      return SSLSTATUS_FAIL;
  }
}


zxy_client_ssl_conn_t* convert_client_ssl_conn(void *ptr)
{
    return (zxy_client_ssl_conn_t*)(((zxy_client_base_t*)ptr)->params);
}

zxy_client_base_t* convert_client_ssl_base(void *ptr)
{
    return (zxy_client_base_t*)ptr;
}

void zxy_queue_encrypted_bytes(zxy_client_ssl_conn_t *client_conn, size_t len)
{
    if (zxy_should_resize_buffer(client_conn->writing_buffer_manager)) {
        zxy_double_buffer_size(client_conn->writing_buffer_manager);
    }

    while (!zxy_can_write_nbytes_to_buffer(client_conn->writing_buffer_manager, len)) {
        zxy_double_buffer_size(client_conn->writing_buffer_manager);
    }
    
    memcpy(
        client_conn->writing_buffer_manager->buffer + client_conn->writing_buffer_manager->current_buffer_ptr,
        client_conn->read_buffer_manager->buffer, len);
    
    zxy_nbyte_written_to_buffer(client_conn->writing_buffer_manager, len);
}

int zxy_on_client_ssl_read_event(void *ptr)
{
    zxy_client_ssl_conn_t *client_conn = convert_client_ssl_conn(ptr);

    int nbytes;

    if (zxy_should_resize_buffer(client_conn->read_buffer_manager)) {
        zxy_double_buffer_size(client_conn->read_buffer_manager);
    }

    zxy_read_io_req_t read_req;

    read_req.req_fd = client_conn->sock_fd;
    read_req.buffer = client_conn->read_buffer_manager->buffer + client_conn->read_buffer_manager->current_buffer_ptr;
    read_req.maximum_read_buffer_size = READ_BUF_SIZE;
    read_req.flags = 0;

    nbytes = zxy_read_socket_non_block(&read_req);

    if (nbytes == 0) {
        LOG_WARNING("we should going to close\n");
        return 0; //TODO: some how signal close state
    } else if (nbytes == WOULD_BLOCK) {
        return WOULD_BLOCK;
    } else if (nbytes == UNKOWN_ERROR) {
        LOG_ERROR("fuck in recv\n");
        return UNKOWN_ERROR;
    }

    int read_nbytes = nbytes;

    zxy_nbyte_written_to_buffer(client_conn->read_buffer_manager, nbytes);

    //proccessing ssl bytes
    char buf[READ_BUF_SIZE];

    int n;
    int base_read = 0;
    enum sslstatus status;

    while (nbytes > 0) {

        n = BIO_write(
            client_conn->rbio,
            client_conn->read_buffer_manager->buffer + base_read,
            nbytes
        );

        if (n < 0)
            return UNKOWN_ERROR;

        base_read += n;
        nbytes -= n;
        
        if (!SSL_is_init_finished(client_conn->ssl)) {
            n = SSL_accept(client_conn->ssl);
            status = get_sslstatus(client_conn->ssl, n);

            if (status == SSLSTATUS_WANT_IO) {
                do {
                    n = BIO_read(client_conn->wbio, buf, sizeof(buf));
                    if (n > 0)
                        zxy_queue_encrypted_bytes(buf, n);
                    else if (!BIO_should_retry(client.wbio))
                        return -1;
                } while(n > 0)
            }

            if (status == SSLSTATUS_FAIL) {
                return UNKOWN_ERROR;
            }

            if (!SSL_is_init_finished(client_conn->ssl))
                return 0;
        }

    }


    return nbytes;
}

int zxy_on_client_ssl_write_event(void *ptr, zxy_write_io_req_t* write_req)
{

}

int zxy_on_client_ssl_close_event(void *ptr)
{

}

zxy_write_io_req_t zxy_client_ssl_request_buffer_reader(void *ptr)
{

}

int zxy_client_ssl_force_close(void *ptr)
{

}

int zxy_client_ssl_is_ready_for_event(u_int32_t events, u_int32_t is_ready, void* ptr)
{

}

void zxy_free_client_ssl(void *ptr)
{

}