/**
 * @file backend_callbacks.h
 */

#ifndef BACKEND_CALLBACKS_H
#define BACKEND_CALLBACKS_H

#include "events/io/epoll_manager.h"
#include "utils/io/io_helper.h"

/**
 * Callback on the read event for the plain connections.
 * \param ptr Pointer to the zxy_backend_base_t.
 * \return Number of read bytes.
 */
int zxy_on_backend_plain_read_event(void *ptr);

/**
 * Callback on the write event for the plain connections.
 * \param ptr Pointer to the zxy_backend_base_t.
 * \param write_req The buffer and associated properties that needed to write in the underlying socket.
 * \return Number of writen bytes.
 */
int zxy_on_backend_plain_write_event(void *ptr, zxy_write_io_req_t* write_req);

/**
 * Callback on the close event for the plain connections.
 * \param ptr Pointer to the zxy_backend_base_t.
 * \return Return 1 if it is success.
 */
int zxy_on_backend_plain_close_event(void *ptr);

/**
 * Get write request from the plain data for writing in another resource.
 * \param ptr Pointer to the zxy_backend_base_t.
 * \return Return zxy_write_io_req_t which contains buffer that other writers might be interst.
 */
zxy_write_io_req_t zxy_backend_plain_request_buffer_reader(void *ptr);

/**
 * Advance the underlying buffer pointer.
 * \param ptr Pointer to the zxy_backend_base_t.
 * \param nbytes The number of bytes should pointer advance.
 * \return Return 0 on success.
 */
int zxy_backend_read_nbytes_from_buffer(void *ptr, int nbytes);

/**
 * Close the connection by force.
 * \param ptr Pointer to the zxy_backend_base_t.
 * \return Return 1 on success, and return 0 when connection is already close.
 */
int zxy_backend_plain_force_close(void *ptr);

/**
 * Check if particular event has been set on the socket.
 * \param events If it has value other than -1 then it will be set on the zxy_backend_conn_t.
 * \param is_ready Check whatever this event has been set on socket or not.
 * \param ptr Pointer to the zxy_backend_base_t.
 * \return Return 1 if the event has been set, and return 0 when event has not been set.
 */
int zxy_backend_plain_is_ready_for_event(u_int32_t events, u_int32_t is_ready, void* ptr);

/**
 * Free allocated resources.
 * \param ptr Pointer to the zxy_backend_base_t.
 */
void zxy_free_backend_plain(void *ptr);

#endif /* BACKEND_CALLBACKS_H */

