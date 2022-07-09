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
#include "connections/conntypes/backend_types.h"
#include "events/peers/ssl/ssl_backend_callbacks.h"
#include "logging/logs.h"
#include "utils/io/io_helper.h"
#include "utils/io/buffer_manager.h"
#include "utils/timer/timers.h"
#include "parsers/http/http_parser.h"

int zxy_backend_proccess_ssl_bytes(zxy_backend_ssl_conn_t *backend_conn, int number_readed_bytes);

int zxy_backend_encrypt_io_req(zxy_backend_ssl_conn_t *backend_conn);

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
      LOG_ERROR("%s\n", ERR_error_string(ERR_get_error(), NULL));
      return SSLSTATUS_FAIL;
  }
}


zxy_backend_ssl_conn_t* convert_backend_ssl_conn(void *ptr)
{
    return (zxy_backend_ssl_conn_t*)(((zxy_backend_base_t*)ptr)->params);
}

zxy_backend_base_t* convert_backend_ssl_base(void *ptr)
{
    return (zxy_backend_base_t*)ptr;
}

//TODO: wtf just happend
// void zxy_backend_queue_encrypted_bytes(zxy_backend_ssl_conn_t *backend_conn, size_t len)
// {
//     if (zxy_should_resize_buffer(backend_conn->writing_buffer_manager)) {
//         zxy_double_buffer_size(backend_conn->writing_buffer_manager);
//     }

//     while (!zxy_can_write_nbytes_to_buffer(backend_conn->writing_buffer_manager, len)) {
//         zxy_double_buffer_size(backend_conn->writing_buffer_manager);
//     }
    
//     memcpy(
//         backend_conn->writing_buffer_manager->buffer + backend_conn->writing_buffer_manager->current_buffer_ptr,
//         backend_conn->read_buffer_manager->buffer, len);
    
//     zxy_nbyte_written_to_buffer(backend_conn->writing_buffer_manager, len);
// }

void zxy_backend_queue_encrypted_bytes(zxy_backend_ssl_conn_t *backend_conn, char *buf, size_t len)
{
    if (zxy_should_resize_buffer(backend_conn->writing_buffer_manager)) {
        zxy_double_buffer_size(backend_conn->writing_buffer_manager);
    }

    while (!zxy_can_write_nbytes_to_buffer(backend_conn->writing_buffer_manager, len)) {
        zxy_double_buffer_size(backend_conn->writing_buffer_manager);
    }


    memcpy(
        backend_conn->writing_buffer_manager->buffer + backend_conn->writing_buffer_manager->current_buffer_ptr, 
        buf, 
        len
    );

    zxy_nbyte_written_to_buffer(backend_conn->writing_buffer_manager, len);
}

void zxy_backend_queue_unencrypted_bytes(zxy_backend_ssl_conn_t *backend_conn, char *buf, size_t len)
{
     if (zxy_should_resize_buffer(backend_conn->encrypt_buffer_manager)) {
        zxy_double_buffer_size(backend_conn->encrypt_buffer_manager);
    }

    while (!zxy_can_write_nbytes_to_buffer(backend_conn->encrypt_buffer_manager, len)) {
        zxy_double_buffer_size(backend_conn->encrypt_buffer_manager);
    }


    memcpy(
        backend_conn->encrypt_buffer_manager->buffer + backend_conn->encrypt_buffer_manager->current_buffer_ptr, 
        buf, 
        len
    );

    zxy_nbyte_written_to_buffer(backend_conn->encrypt_buffer_manager, len);
}

int zxy_on_backend_ssl_read_event(void *ptr)
{
    zxy_backend_ssl_conn_t *backend_conn = convert_backend_ssl_conn(ptr);

    int nbytes;

    if (zxy_should_resize_buffer(backend_conn->read_buffer_manager)) {
        zxy_double_buffer_size(backend_conn->read_buffer_manager);
    }

    zxy_read_io_req_t read_req;

    read_req.req_fd = backend_conn->sock_fd;
    read_req.buffer = backend_conn->read_buffer_manager->buffer + backend_conn->read_buffer_manager->current_buffer_ptr;
    read_req.maximum_read_buffer_size = READ_BUF_SIZE;
    read_req.flags = 0;

    nbytes = zxy_read_socket_non_block(&read_req);

    LOG_INFO("%d: %d\n", backend_conn->sock_fd, nbytes);
    // fflush(stdout);

    if (nbytes == 0) {
        LOG_WARNING("Read 0 bytes from FD(%d) and we should close\n", backend_conn->sock_fd);
        return 0; //TODO: some how signal close state
    } else if (nbytes == WOULD_BLOCK) {
        LOG_ERROR("We cannot read from FD(%d) because it blocks\n", backend_conn->sock_fd);
        return WOULD_BLOCK;
    } else if (nbytes == UNKOWN_ERROR) {
        LOG_ERROR("We encounte unkown error from FD(%d)\n", backend_conn->sock_fd);
        return UNKOWN_ERROR;
    }

    int read_nbytes = nbytes;

    zxy_nbyte_written_to_buffer(backend_conn->read_buffer_manager, nbytes);

    LOG_INFO("Total bytes until know for FD(%d): %d\n", backend_conn->sock_fd, backend_conn->read_buffer_manager->current_buffer_ptr);

    int result = zxy_backend_proccess_ssl_bytes(backend_conn, read_nbytes);

    // zxy_clean_nbytes_from_buffer(backend_conn->read_buffer_manager, backend_conn->read_buffer_manager->current_buffer_ptr);
    // backend_conn->read_buffer_manager->current_buffer_ptr = 0;

    memmove(
        backend_conn->read_buffer_manager->buffer, 
        backend_conn->read_buffer_manager->buffer + nbytes, 
        backend_conn->read_buffer_manager->current_buffer_ptr - nbytes
    );
    zxy_nbyte_readed_from_buffer(backend_conn->read_buffer_manager, nbytes);

    if (result) {
        LOG_ERROR("We cannot proccess ssl bytes for FD(%d)\n", backend_conn->sock_fd);
    }

    return nbytes;
}

int zxy_backend_proccess_ssl_bytes(zxy_backend_ssl_conn_t *backend_conn, int number_readed_bytes)
{
    //proccessing ssl bytes
    //TODO: eliminate getting large chunk on stack
    char buf[READ_BUF_SIZE];

    int n;
    int base_read_ptr = 0;
    enum sslstatus status;


    LOG_INFO("go read %d\n", number_readed_bytes);

    while (number_readed_bytes > 0) {

        n = BIO_write(
            backend_conn->rbio,
            backend_conn->read_buffer_manager->buffer + base_read_ptr,
            number_readed_bytes
        );

        if (n <= 0) {
            LOG_ERROR("BIO write error for FD(%d)\n", backend_conn->sock_fd);
            return UNKOWN_ERROR;
        }

        base_read_ptr += n;
        number_readed_bytes -= n;

        LOG_INFO("Going to read %d from FD(%d)\n", n, backend_conn->sock_fd);
        
        if (!SSL_is_init_finished(backend_conn->ssl)) {
            n = SSL_connect(backend_conn->ssl);

            if (n < 0) {
                int error = SSL_get_error(backend_conn->ssl, n);
                LOG_ERROR("%d %s\n", error, ERR_error_string(ERR_get_error(), NULL));
            }


            status = get_sslstatus(backend_conn->ssl, n);

            LOG_INFO("Retrun from SSL_connect for FD(%d): %d\n", backend_conn->sock_fd, n);

            if (status == SSLSTATUS_WANT_IO) {
                do {
                    n = BIO_read(backend_conn->wbio, buf, sizeof(buf));
                    if (n > 0)
                        zxy_backend_queue_encrypted_bytes(backend_conn, buf, n);
                    else if (!BIO_should_retry(backend_conn->wbio)) {
                        LOG_ERROR("Error wbio for FD(%d)\n", backend_conn->sock_fd);
                        return -1;
                    }
                } while(n > 0);
            }


            if (status == SSLSTATUS_FAIL) {
                LOG_ERROR("Unkown error for FD(%d)\n", backend_conn->sock_fd);
                return UNKOWN_ERROR;
            }

            LOG_INFO("Final bytes for FD(%d) is %d\n", backend_conn->sock_fd, n);

            if (!SSL_is_init_finished(backend_conn->ssl))
                return 0;
            backend_conn->is_ssl_handshake_done = 1;
        }

        LOG_INFO("Going to SSL_read for FD(%d)\n", backend_conn->sock_fd);

        do {
            n = SSL_read(backend_conn->ssl, buf, sizeof(buf));
            // if (n > 0)
            //     backend.io_on_read(buf, (size_t)n);
            if (n > 0) {
                if (zxy_should_resize_buffer(backend_conn->plain_buffer_manager) || !zxy_can_write_nbytes_to_buffer(backend_conn->plain_buffer_manager, n)) {
                    zxy_double_buffer_size(backend_conn->plain_buffer_manager);
                }

                memcpy(
                    backend_conn->plain_buffer_manager->buffer + backend_conn->plain_buffer_manager->current_buffer_ptr, 
                    buf, 
                    n
                );

                zxy_nbyte_written_to_buffer(backend_conn->plain_buffer_manager, n);


                // LOG_INFO("soo: %.*s\n", (int)n, buf);
            }

        } while (n > 0);

        status = get_sslstatus(backend_conn->ssl, n);

        /* Did SSL request to write bytes? This can happen if peer has requested SSL
        * renegotiation. */
        if (status == SSLSTATUS_WANT_IO)
        do {
            n = BIO_read(backend_conn->wbio, buf, sizeof(buf));
            if (n > 0)
                zxy_backend_queue_encrypted_bytes(backend_conn, buf, n);
            else if (!BIO_should_retry(backend_conn->wbio)) {
                LOG_ERROR("Error wbio for FD(%d)\n", backend_conn->sock_fd);
                return UNKOWN_ERROR;
            }
        } while (n>0);

        if (status == SSLSTATUS_FAIL) {
            LOG_ERROR("Unkown error for FD(%d)\n", backend_conn->sock_fd);
            return UNKOWN_ERROR;
        }

    }

    return 0;
}

int zxy_on_backend_ssl_write_event(void *ptr, zxy_write_io_req_t* write_req)
{
    zxy_backend_ssl_conn_t *backend_conn = convert_backend_ssl_conn(ptr);

    int ssl_nbytes;
    int nbytes;
        
    write_req->req_fd = backend_conn->sock_fd;

    LOG_INFO("Write req for FD(%d): number of bytes is %d and current and max buffer size: %d and current_buffer_ptr %d\n",
        backend_conn->sock_fd,
        write_req->send_nbytes, 
        backend_conn->encrypt_buffer_manager->max_size_of_buffer,
        backend_conn->encrypt_buffer_manager->current_buffer_ptr);

    //if (likely(backend_conn->is_ssl_handshake_done == 1))
    //{
      int pret;
      struct http_request_details http_req;
      http_req.http_commons.http_buffer = write_req->buffer;
      http_req.http_commons.size_of_http_buffer = write_req->send_nbytes;
      http_req.http_commons.max_buffer_size = backend_conn->encrypt_buffer_manager->max_size_of_buffer;

      pret = zxy_parse_http_request(&http_req);
      if (pret > 0)
      {
        LOG_INFO("Method is %.*s\n", (int)http_req.method_len, http_req.method);

      struct http_header header;
      header.name = "Host\0";
      header.name_len = strlen(header.name);
      header.value = "quera.org\0";
      header.value_len = strlen(header.value);
      pret = zxy_modify_http_headers(&http_req.http_commons, &header);
      write_req->send_nbytes += pret;
      LOG_INFO("Modified Req is \n%s\n", write_req->buffer);
      }
    //}

    zxy_backend_queue_unencrypted_bytes(backend_conn, write_req->buffer, write_req->send_nbytes);
    nbytes = write_req->send_nbytes;
    
    zxy_backend_encrypt_io_req(backend_conn);

    write_req->buffer = backend_conn->writing_buffer_manager->buffer;
    write_req->send_nbytes = backend_conn->writing_buffer_manager->current_buffer_ptr;
    write_req->clear_nbytes = backend_conn->writing_buffer_manager->current_buffer_ptr;

    if (write_req->send_nbytes <= 0) {
        LOG_INFO("Send bytes is under zero for FD(%d)\n", backend_conn->sock_fd);
        return 0;
    }
    
    ssl_nbytes = zxy_write_socket_non_block_and_clear_buf(write_req);

    memmove(
        backend_conn->writing_buffer_manager->buffer, 
        backend_conn->writing_buffer_manager->buffer + ssl_nbytes, 
        backend_conn->writing_buffer_manager->current_buffer_ptr - ssl_nbytes
    );
    zxy_nbyte_readed_from_buffer(backend_conn->writing_buffer_manager, ssl_nbytes);

    if (ssl_nbytes == 0) {
        LOG_WARNING("Read 0 bytes from FD(%d) we should close the backend\n", backend_conn->sock_fd);
        return 0;
    } else if (ssl_nbytes == WOULD_BLOCK) {
        return WOULD_BLOCK;
    } else if (ssl_nbytes == UNKOWN_ERROR) {
        LOG_ERROR("We encounter UNKOWN ERROR is FD(%d)\n", backend_conn->sock_fd);
        return UNKOWN_ERROR;
    }

    LOG_INFO("Number of written bytes in FD(%d) is %d\n", backend_conn->sock_fd, nbytes);
    
    return nbytes;
}

int zxy_backend_encrypt_io_req(zxy_backend_ssl_conn_t *backend_conn)
{
    char buf[READ_BUF_SIZE];
    enum sslstatus status;
    int n;
    if (!SSL_is_init_finished(backend_conn->ssl)) {
      LOG_ERROR("heloooooooooooo\n");
      n = SSL_connect(backend_conn->ssl);

            if (n < 0) {
                int error = SSL_get_error(backend_conn->ssl, n);
                LOG_ERROR("%d %s\n", error, ERR_error_string(ERR_get_error(), NULL));
            }


            status = get_sslstatus(backend_conn->ssl, n);

            LOG_INFO("Retrun from SSL_connect for FD(%d): %d\n", backend_conn->sock_fd, n);

            if (status == SSLSTATUS_WANT_IO) {
                do {
                    n = BIO_read(backend_conn->wbio, buf, sizeof(buf));
                    if (n > 0)
                        zxy_backend_queue_encrypted_bytes(backend_conn, buf, n);
                    else if (!BIO_should_retry(backend_conn->wbio)) {
                        LOG_ERROR("Error wbio for FD(%d)\n", backend_conn->sock_fd);
                        return -1;
                    }
                } while(n > 0);
            }


            if (status == SSLSTATUS_FAIL) {
                LOG_ERROR("Unkown error for FD(%d)\n", backend_conn->sock_fd);
                return UNKOWN_ERROR;
            }

            LOG_INFO("Final bytes for FD(%d) is %d\n", backend_conn->sock_fd, n);

            if (!SSL_is_init_finished(backend_conn->ssl))
                return 0;
            backend_conn->is_ssl_handshake_done = 1;
    }

    while (backend_conn->encrypt_buffer_manager->current_buffer_ptr >0) {
        int n = SSL_write(
            backend_conn->ssl, 
            backend_conn->encrypt_buffer_manager->buffer, 
            backend_conn->encrypt_buffer_manager->current_buffer_ptr);
        status = get_sslstatus(backend_conn->ssl, n);

        if (n > 0) {
            /* consume the waiting bytes that have been used by SSL */
            if ((size_t)n < backend_conn->encrypt_buffer_manager->current_buffer_ptr) {
                memmove(
                    backend_conn->encrypt_buffer_manager->buffer, 
                    backend_conn->encrypt_buffer_manager->buffer + n, 
                    backend_conn->encrypt_buffer_manager->current_buffer_ptr - n
                );
            }

            zxy_nbyte_readed_from_buffer(backend_conn->encrypt_buffer_manager, n);

            // zxy_resize_to_prefer_buffer_size(
            //     backend_conn->encrypt_buffer_manager, 
            //     backend_conn->encrypt_buffer_manager->current_buffer_ptr);

            /* take the output of the SSL object and queue it for socket write */
            do {
                n = BIO_read(backend_conn->wbio, buf, sizeof(buf));
                if (n > 0) {
                    zxy_backend_queue_encrypted_bytes(backend_conn, buf, n);
                }
                else if (!BIO_should_retry(backend_conn->wbio)) {
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

int zxy_on_backend_ssl_close_event(void *ptr)
{
    zxy_backend_ssl_conn_t *backend_conn = convert_backend_ssl_conn(ptr);

    zxy_remove_fd_from_epoll(backend_conn->sock_fd);
    LOG_INFO("Client FD(%d) is going to close\n", backend_conn->sock_fd);
    backend_conn->is_closed = 1;
    close(backend_conn->sock_fd);

    return 1;
}

zxy_write_io_req_t zxy_backend_ssl_request_buffer_reader(void *ptr)
{
    zxy_backend_ssl_conn_t *backend_conn = convert_backend_ssl_conn(ptr);

    zxy_write_io_req_t write_req;
    write_req.buffer = backend_conn->plain_buffer_manager->buffer;
    write_req.flags = 0;
    write_req.send_nbytes = backend_conn->plain_buffer_manager->current_buffer_ptr;
    write_req.clear_nbytes = backend_conn->plain_buffer_manager->current_buffer_ptr;
    
    return write_req;
}

int zxy_backend_ssl_read_nbytes_from_buffer(void *ptr, int nbytes)
{
  if (nbytes <= 0)
    return -1;
  
  zxy_backend_ssl_conn_t *backend_conn = convert_backend_ssl_conn(ptr);
  
  memmove(
    backend_conn->plain_buffer_manager->buffer, 
    backend_conn->plain_buffer_manager->buffer + nbytes, 
    backend_conn->plain_buffer_manager->current_buffer_ptr - nbytes
  );
  zxy_nbyte_readed_from_buffer(backend_conn->plain_buffer_manager, nbytes);

  return 0;
}

int zxy_backend_ssl_force_close(void *ptr)
{
    zxy_backend_ssl_conn_t *backend_conn = convert_backend_ssl_conn(ptr);

    if (backend_conn->is_closed != 1) {
        zxy_remove_fd_from_epoll(backend_conn->sock_fd);
        LOG_INFO("Client FD(%d) closed the connection\n", backend_conn->sock_fd);
        backend_conn->is_closed = 1;
        close(backend_conn->sock_fd);

        return 1;
    }

    return 0;
}

int zxy_backend_ssl_is_ready_for_event(u_int32_t events, u_int32_t is_ready, void* ptr)
{
    zxy_backend_ssl_conn_t *backend_conn = convert_backend_ssl_conn(ptr);

    if (events != -1) {
        backend_conn->events = events;
    }

    switch (is_ready)
    {
    case READ_EVENT: {
        if (backend_conn->events & EPOLLIN) return 1;
        return 0;
        break;
    }
    case WRITE_EVENT: {
        if (backend_conn->events & EPOLLOUT) return 1;
        return 0;
        break;
    }
    case CLOSE_EVENT: {
        if ((backend_conn->events & EPOLLHUP) | (backend_conn->events & EPOLLERR)) return 1;
        return 0;
        break;
    }
    
    default:
        return 0;
    }
}

void zxy_free_backend_ssl(void *ptr)
{
    zxy_backend_base_t *backend_base = convert_backend_ssl_base(ptr);
    zxy_backend_ssl_conn_t *backend_conn = convert_backend_ssl_conn(ptr);

    zxy_free_buffer_manager(backend_conn->read_buffer_manager);
    zxy_free_buffer_manager(backend_conn->plain_buffer_manager);
    zxy_free_buffer_manager(backend_conn->writing_buffer_manager);
    zxy_free_buffer_manager(backend_conn->encrypt_buffer_manager);

    SSL_free(backend_conn->ssl);
    
    free(backend_conn);

    backend_base->set_free = 1;
}

