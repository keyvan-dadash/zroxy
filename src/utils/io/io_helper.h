#ifndef IO_HELPER_H
#define IO_HELPER_H

#include <sys/types.h>

#define WOULD_BLOCK -1
#define UNKOWN_ERROR -2

/**
 * Read request details.
 */
typedef struct
{
    /**
     * The FD for doing read operation.
     */
    int req_fd;

    /**
     * Read from FD and save in the buffer.
     */
    char *buffer;

    /**
     * Maximum size of buffer.
     */
    int32_t maximum_read_buffer_size;

    /**
     * Flags for the associated FD in read syscall.
     */
    int flags;
} zxy_read_io_req_t;

/**
 * Write request details.
 */
typedef struct
{
    /**
     * The FD for doing write operation.
     */
    int req_fd;

    /**
     * Write bytes from buffer to FD.
     */
    char *buffer;

    /**
     * Number of bytes to send in FD.
     */
    int32_t send_nbytes;

    /**
     * Number of bytes to clean after write operation.
     */
    int32_t clear_nbytes;

    /**
     * Flags for the associated FD in write syscall.
     */
    int flags;
} zxy_write_io_req_t;

/**
 * Read from the socket in non-block mode.
 * \param io_req The read operation details.
 * \return Return number of the read bytes, or return -1 on block, or return -2 on error.
 */
int zxy_read_socket_non_block(zxy_read_io_req_t *io_req);

/**
 * Write to the socket in non-block mode.
 * \param io_req The write operation details.
 * \return Return number of the written bytes, or return -1 on block, or return -2 on error.
 */
int zxy_write_socket_non_block(zxy_write_io_req_t *io_req);

/**
 * Write to the socket in non-block mode, and clean bytes.
 * \param io_req The write operation details.
 * \return Return number of the written bytes, or return -1 on block, or return -2 on error.
 */
int zxy_write_socket_non_block_and_clear_buf(zxy_write_io_req_t *io_req);

/**
 * Check if the event readable.
 * \param event The event to check.
 * \return Return 1 if the event readable.
 */
int8_t zxy_is_readable_event(u_int32_t event);

/**
 * Check if the event writable.
 * \param event The event to check.
 * \return Return 1 if the event writable.
 */
int8_t zxy_is_writable_event(u_int32_t event);

#endif /* IO_HELPER_H */

