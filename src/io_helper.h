


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
} read_io_req_t;

typedef struct
{
    int req_fd;
    char *buffer;
    int32_t send_nbytes;
    int32_t clear_nbytes;
    int flags;
} write_io_req_t;


int read_socket_non_block(read_io_req_t *io_req);

int write_socket_non_block(write_io_req_t *io_req);

int write_socket_non_block_and_clear_buf(write_io_req_t *io_req);

int8_t is_readable_event(u_int32_t);

int8_t is_writable_event(u_int32_t);




#endif /* IO_HELPER_H */