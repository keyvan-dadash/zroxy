



#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>

#include <openssl/err.h>

#include "defines.h"

#include "connections/conntypes/client_types.h"
#include "events/peers/ssl/ssl_client_callbacks.h"
#include "logging/logs.h"
#include "utils/io/io_helper.h"
#include "utils/io/buffer_manager.h"
#include "utils/timer/timers.h"


int zxy_proccess_ssl_bytes(zxy_client_ssl_conn_t *client_conn, int number_readed_bytes);

int zxy_encrypt_io_req(zxy_client_ssl_conn_t *client_conn);

enum sslstatus { SSLSTATUS_OK, SSLSTATUS_WANT_IO, SSLSTATUS_FAIL};

static enum sslstatus get_sslstatus(SSL* ssl, int n)
{

  int error = SSL_get_error(ssl, n);

  switch (error)
  {
    case SSL_ERROR_NONE:
      return SSLSTATUS_OK;
    case SSL_ERROR_WANT_WRITE:
    case SSL_ERROR_WANT_READ:
      return SSLSTATUS_WANT_IO;
    case SSL_ERROR_ZERO_RETURN:
    case SSL_ERROR_SYSCALL:
    default:
      LOG_INFO("%s\n", ERR_error_string(ERR_get_error(), NULL));
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

//TODO: wtf just happend
// void zxy_queue_encrypted_bytes(zxy_client_ssl_conn_t *client_conn, size_t len)
// {
//     if (zxy_should_resize_buffer(client_conn->writing_buffer_manager)) {
//         zxy_double_buffer_size(client_conn->writing_buffer_manager);
//     }

//     while (!zxy_can_write_nbytes_to_buffer(client_conn->writing_buffer_manager, len)) {
//         zxy_double_buffer_size(client_conn->writing_buffer_manager);
//     }
    
//     memcpy(
//         client_conn->writing_buffer_manager->buffer + client_conn->writing_buffer_manager->current_buffer_ptr,
//         client_conn->read_buffer_manager->buffer, len);
    
//     zxy_nbyte_written_to_buffer(client_conn->writing_buffer_manager, len);
// }

void zxy_queue_encrypted_bytes(zxy_client_ssl_conn_t *client_conn, char *buf, size_t len)
{
     if (zxy_should_resize_buffer(client_conn->writing_buffer_manager)) {
        zxy_double_buffer_size(client_conn->writing_buffer_manager);
    }

    while (!zxy_can_write_nbytes_to_buffer(client_conn->writing_buffer_manager, len)) {
        zxy_double_buffer_size(client_conn->writing_buffer_manager);
    }


    memcpy(
        client_conn->writing_buffer_manager->buffer + client_conn->writing_buffer_manager->current_buffer_ptr, 
        buf, 
        len
    );

    zxy_nbyte_written_to_buffer(client_conn->writing_buffer_manager, len);
}

void zxy_queue_unencrypted_bytes(zxy_client_ssl_conn_t *client_conn, char *buf, size_t len)
{
     if (zxy_should_resize_buffer(client_conn->encrypt_buffer_manager)) {
        zxy_double_buffer_size(client_conn->encrypt_buffer_manager);
    }

    while (!zxy_can_write_nbytes_to_buffer(client_conn->encrypt_buffer_manager, len)) {
        zxy_double_buffer_size(client_conn->encrypt_buffer_manager);
    }


    memcpy(
        client_conn->encrypt_buffer_manager->buffer + client_conn->encrypt_buffer_manager->current_buffer_ptr, 
        buf, 
        len
    );

    zxy_nbyte_written_to_buffer(client_conn->encrypt_buffer_manager, len);
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

    LOG_INFO("%d: %d\n", client_conn->sock_fd, nbytes);
    // fflush(stdout);

    if (nbytes == 0) {
        LOG_WARNING("we should going to close(ssl client)\n");
        return 0; //TODO: some how signal close state
    } else if (nbytes == WOULD_BLOCK) {
        LOG_ERROR("fuck in recv11\n");
        return WOULD_BLOCK;
    } else if (nbytes == UNKOWN_ERROR) {
        LOG_ERROR("fuck in recv\n");
        return UNKOWN_ERROR;
    }

    int read_nbytes = nbytes;

    zxy_nbyte_written_to_buffer(client_conn->read_buffer_manager, nbytes);

    LOG_INFO("total bytes: %d\n", client_conn->read_buffer_manager->current_buffer_ptr);

    int result = zxy_proccess_ssl_bytes(client_conn, read_nbytes);

    // zxy_clean_nbytes_from_buffer(client_conn->read_buffer_manager, client_conn->read_buffer_manager->current_buffer_ptr);
    // client_conn->read_buffer_manager->current_buffer_ptr = 0;

    memmove(
        client_conn->read_buffer_manager->buffer, 
        client_conn->read_buffer_manager->buffer + nbytes, 
        client_conn->read_buffer_manager->current_buffer_ptr - nbytes
    );
    zxy_nbyte_readed_from_buffer(client_conn->read_buffer_manager, nbytes);

    if (result) {
        LOG_ERROR("something bad happend\n");
    }

    return nbytes;
}

int zxy_proccess_ssl_bytes(zxy_client_ssl_conn_t *client_conn, int number_readed_bytes)
{
    //proccessing ssl bytes
    //TODO: eliminate getting large chunk on stack
    char buf[READ_BUF_SIZE];

    int n;
    int base_read_ptr = 0;
    enum sslstatus status;


    // LOG_INFO("go read %d\n", number_readed_bytes);

    while (number_readed_bytes > 0) {

        n = BIO_write(
            client_conn->rbio,
            client_conn->read_buffer_manager->buffer + base_read_ptr,
            number_readed_bytes
        );

        if (n <= 0) {
            LOG_INFO("bad bad\n");
            return UNKOWN_ERROR;
        }

        base_read_ptr += n;
        number_readed_bytes -= n;

        LOG_INFO("go read %d\n", n);
        
        if (!SSL_is_init_finished(client_conn->ssl)) {
            n = SSL_accept(client_conn->ssl);

            if (n < 0) {
                int error = SSL_get_error(client_conn->ssl, n);
                LOG_INFO("%d %s\n", error, ERR_error_string(ERR_get_error(), NULL));
            }


            status = get_sslstatus(client_conn->ssl, n);

            LOG_INFO("before babababababab1111 %d\n", n);

            if (status == SSLSTATUS_WANT_IO) {
                do {
                    n = BIO_read(client_conn->wbio, buf, sizeof(buf));
                    if (n > 0)
                        zxy_queue_encrypted_bytes(client_conn, buf, n);
                    else if (!BIO_should_retry(client_conn->wbio)) {
                        LOG_INFO("bad bad1\n");
                        return -1;
                    }
                } while(n > 0);
            }


            if (status == SSLSTATUS_FAIL) {
                LOG_INFO("bad bad2\n");
                return UNKOWN_ERROR;
            }

            LOG_INFO("babababababab1111 %d\n", n);

            if (!SSL_is_init_finished(client_conn->ssl))
                return 0;
        }

        LOG_INFO("babababababab\n");

        do {
            n = SSL_read(client_conn->ssl, buf, sizeof(buf));
            // if (n > 0)
            //     client.io_on_read(buf, (size_t)n);
            if (n > 0)
                LOG_INFO("soo: %.*s\n", (int)n, buf);
        } while (n > 0);

        status = get_sslstatus(client_conn->ssl, n);

        /* Did SSL request to write bytes? This can happen if peer has requested SSL
        * renegotiation. */
        if (status == SSLSTATUS_WANT_IO)
        do {
            n = BIO_read(client_conn->wbio, buf, sizeof(buf));
            if (n > 0)
                zxy_queue_encrypted_bytes(client_conn, buf, n);
            else if (!BIO_should_retry(client_conn->wbio)) {
                LOG_INFO("bad bad3\n");
                return UNKOWN_ERROR;
            }
        } while (n>0);

        if (status == SSLSTATUS_FAIL) {
            LOG_INFO("bad bad3\n");
            return UNKOWN_ERROR;
        }

    }

    return 0;
}

int zxy_on_client_ssl_write_event(void *ptr, zxy_write_io_req_t* write_req)
{
    zxy_client_ssl_conn_t *client_conn = convert_client_ssl_conn(ptr);

    int nbytes;
        
    write_req->req_fd = client_conn->sock_fd;

    LOG_INFO("write req: %d\n", write_req->send_nbytes);

    zxy_queue_unencrypted_bytes(client_conn, write_req->buffer, write_req->send_nbytes);

    zxy_encrypt_io_req(client_conn);

    write_req->buffer = client_conn->writing_buffer_manager->buffer;
    write_req->send_nbytes = client_conn->writing_buffer_manager->current_buffer_ptr;
    write_req->clear_nbytes = client_conn->writing_buffer_manager->current_buffer_ptr;

    if (write_req->send_nbytes <= 0) {
        LOG_INFO("wtfff dude\n");
        return 0;
    }

    nbytes = zxy_write_socket_non_block_and_clear_buf(write_req);

    // client_conn->writing_buffer_manager->current_buffer_ptr = 0;

    memmove(
        client_conn->writing_buffer_manager->buffer, 
        client_conn->writing_buffer_manager->buffer + nbytes, 
        client_conn->writing_buffer_manager->current_buffer_ptr - nbytes
    );
    zxy_nbyte_readed_from_buffer(client_conn->writing_buffer_manager, nbytes);

    if (nbytes == 0) {
        LOG_WARNING("we should going to close(ssl client)\n");
        return 0;
    } else if (nbytes == WOULD_BLOCK) {
        return WOULD_BLOCK;
    } else if (nbytes == UNKOWN_ERROR) {
        LOG_ERROR("fuck in send\n");
        return UNKOWN_ERROR;
    }

    LOG_INFO("number of written bytes is %d(in ssl send client)\n", nbytes);
    
    return nbytes;
}

int zxy_encrypt_io_req(zxy_client_ssl_conn_t *client_conn)
{
    char buf[READ_BUF_SIZE];
    enum sslstatus status;

    if (!SSL_is_init_finished(client_conn->ssl))
        return 0;

    while (client_conn->encrypt_buffer_manager->current_buffer_ptr >0) {
        int n = SSL_write(
            client_conn->ssl, 
            client_conn->encrypt_buffer_manager->buffer, 
            client_conn->encrypt_buffer_manager->current_buffer_ptr);
        status = get_sslstatus(client_conn->ssl, n);

        if (n > 0) {
            /* consume the waiting bytes that have been used by SSL */
            if ((size_t)n < client_conn->encrypt_buffer_manager->current_buffer_ptr) {
                memmove(
                    client_conn->encrypt_buffer_manager->buffer, 
                    client_conn->encrypt_buffer_manager->buffer + n, 
                    client_conn->encrypt_buffer_manager->current_buffer_ptr - n
                );
            }

            zxy_nbyte_readed_from_buffer(client_conn->encrypt_buffer_manager, n);

            zxy_resize_to_prefer_buffer_size(
                client_conn->encrypt_buffer_manager, 
                client_conn->encrypt_buffer_manager->current_buffer_ptr);

            /* take the output of the SSL object and queue it for socket write */
            do {
                n = BIO_read(client_conn->wbio, buf, sizeof(buf));
                if (n > 0) {
                    zxy_queue_encrypted_bytes(client_conn, buf, n);
                }
                else if (!BIO_should_retry(client_conn->wbio)) {
                    return -1;
                }
            } while (n>0);
        }

        if (status == SSLSTATUS_FAIL)
            return -1;

        if (n==0)
            break;
    }
    return 0;
}

int zxy_on_client_ssl_close_event(void *ptr)
{
    zxy_client_ssl_conn_t *client_conn = convert_client_ssl_conn(ptr);

    zxy_remove_fd_from_epoll(client_conn->sock_fd);
    LOG_INFO("client fd(%d) closed the connection\n", client_conn->sock_fd);
    client_conn->is_closed = 1;
    close(client_conn->sock_fd);

    return 1;
}

zxy_write_io_req_t zxy_client_ssl_request_buffer_reader(void *ptr)
{
    zxy_client_ssl_conn_t *client_conn = convert_client_ssl_conn(ptr);

    zxy_write_io_req_t write_req;
    write_req.buffer = client_conn->encrypt_buffer_manager->buffer;
    write_req.flags = 0;
    write_req.send_nbytes = client_conn->encrypt_buffer_manager->current_buffer_ptr;
    write_req.clear_nbytes = client_conn->encrypt_buffer_manager->current_buffer_ptr;
    
    return write_req;
}

int zxy_client_ssl_force_close(void *ptr)
{
    zxy_client_ssl_conn_t *client_conn = convert_client_ssl_conn(ptr);

    if (client_conn->is_closed != 1) {
        zxy_remove_fd_from_epoll(client_conn->sock_fd);
        LOG_INFO("backend fd(%d) closed the connection\n", client_conn->sock_fd);
        client_conn->is_closed = 1;
        close(client_conn->sock_fd);

        return 1;
    }

    return 0;
}

int zxy_client_ssl_is_ready_for_event(u_int32_t events, u_int32_t is_ready, void* ptr)
{
    zxy_client_ssl_conn_t *client_conn = convert_client_ssl_conn(ptr);

    if (events != -1) {
        client_conn->events = events;
    }

    switch (is_ready)
    {
    case READ_EVENT: {
        if (client_conn->events & EPOLLIN) return 1;
        return 0;
        break;
    }
    case WRITE_EVENT: {
        if (client_conn->events & EPOLLOUT) return 1;
        return 0;
        break;
    }
    case CLOSE_EVENT: {
        if ((client_conn->events & EPOLLHUP) | (client_conn->events & EPOLLERR)) return 1;
        return 0;
        break;
    }
    
    default:
        return 0;
    }
}

void zxy_free_client_ssl(void *ptr)
{
    zxy_client_base_t *client_base = convert_client_ssl_base(ptr);
    zxy_client_ssl_conn_t *client_conn = convert_client_ssl_conn(ptr);

    zxy_free_buffer_manager(client_conn->read_buffer_manager);
    zxy_free_buffer_manager(client_conn->writing_buffer_manager);
    zxy_free_buffer_manager(client_conn->encrypt_buffer_manager);

    SSL_free(client_conn->ssl);
    
    free(client_conn);

    client_base->set_free = 1;
}