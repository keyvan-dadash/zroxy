#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <sys/types.h>

/* similar to this link
https://embeddedartistry.com/blog/2017/05/17/creating-a-circular-buffer-in-c-and-c/
*/

/**
 * Circular buffer properties.
 */
typedef struct
{
    /**
     * Buffer of the circular buffer.
     */
    char *buf;

    /**
     * Tail of the buffer where the latest byte stored in the buffer.
     */
    size_t tail;

    /**
     * Head of the buffer where the first byte sotred in the buffer.
     */
    size_t head;

    /**
     * Maximum size of the circular buffer.
     */
    size_t maximum_size_of_buffer;

    /**
     * Shows if the circular buffer grows at it maximum size.
     */
    int8_t is_full;
} zxy_circular_buffer_t;

/**
 * Create circualr buffer.
 * \param size_of_buffer Intial size of the buffer.
 * \return Return instance of zxy_circular_buffer_t.
 */
zxy_circular_buffer_t *zxy_create_circular_buffer(int32_t size_of_buffer);

/**
 * Clear bytes in the circualr buffer.
 * \param circular_buff Pointer to the instance of the circualr buffer.
 * \return Return 1 on success.
 */
int8_t zxy_clear_circular_buffer(zxy_circular_buffer_t *circular_buff);

/**
 * Free allocated resource.
 * \param circular_buff Pointer to the instance of the circualr buffer.
 * \return Return 1 on success.
 */
int8_t zxy_free_circular_buffer(zxy_circular_buffer_t *circular_buff);

/**
 * Write buffer bytes into the circualr buffer.
 * \param circular_buff Pointer to the instance of the circualr buffer.
 * \param buf The buffer which bytes is going to be written inside the circualr buffer.
 * \param len Size of the buffer.
 * \return Number of written bytes.
 */
int32_t zxy_write_buffer_to_circular_buffer(zxy_circular_buffer_t *circular_buff, char *buf, size_t len);

/**
 * Get buffer from the circualr buffer.
 * \param circular_buff Pointer to the instance of the circualr buffer.
 * \param data The buffer which is going to be fill.
 * \param len Size of the buffer.
 * \return Number read bytes.
 */
int8_t zxy_get_buffer_from_circular_buffer(zxy_circular_buffer_t *circular_buff, char *data, size_t len);

/**
 * Check if the circualr buffer empty or not.
 * \param circular_buff Pointer to the instance of the circualr buffer.
 * \return Reutrn 1 if the circualr buffer empty.
 */
int8_t zxy_is_circular_buffer_empty(zxy_circular_buffer_t *circular_buff);

/**
 * Return size of bytes inside the circualr buffer.
 * \param circular_buff Pointer to the instance of the circualr buffer.
 * \return Return size of circualr buffer.
 */
int32_t zxy_current_circular_buffer_size(zxy_circular_buffer_t *circular_buff);

#endif /* CIRCULAR_BUFFER_H */

