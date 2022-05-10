


#ifndef IO_HELPER_H
#define IO_HELPER_H

#include <sys/types.h>


#define WOULD_BLOCK -1
#define UNKOWN_ERROR -2

typedef struct
{
    int req_fd;
    char *buffer;
    int32_t maximum_read_buffer_size;
    int flags;
} zxy_read_io_req_t;

typedef struct
{
    int req_fd;
    char *buffer;
    int32_t send_nbytes;
    int32_t clear_nbytes;
    int flags;
} zxy_write_io_req_t;


int zxy_read_socket_non_block(zxy_read_io_req_t *io_req);

int zxy_write_socket_non_block(zxy_write_io_req_t *io_req);

int zxy_write_socket_non_block_and_clear_buf(zxy_write_io_req_t *io_req);

int8_t zxy_is_readable_event(u_int32_t);

int8_t zxy_is_writable_event(u_int32_t);




#endif /* IO_HELPER_H */
