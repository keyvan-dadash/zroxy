#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <sys/types.h>

/**
 * Buffer manager properties.
 */
typedef struct
{
    /**
     * Underlying buffer.
     */
    char *buffer;

    /**
     * Maximum size of the buffer.
     */
    int max_size_of_buffer;

    /**
     * Buffer ptr which is represent at where should write bytes to the buffer.
     */ 
    int current_buffer_ptr;
} zxy_buffer_manager_t;

/**
 * Create and allocate buffer manager.
 * \param max_buffer_size The size of the underlying buffer.
 * \return Return buffer manager struct.
 */
zxy_buffer_manager_t* zxy_malloc_buffer_manager_with_buffer_size(int max_buffer_size);

/**
 * Make size of the buffer inside the buffer manager double.
 * \param buffer_manager Pointer to the buffer manager.
 * \return Return size of the new buffer.
 */
int zxy_double_buffer_size(zxy_buffer_manager_t* buffer_manager);

/**
 * Resize the underlying buffer to prefer size.
 * \param buffer_manager Pointer to the buffer manager.
 * \param prefer_size Size of the new buffer.
 * \return Return size of the new buffer.
 */
int zxy_resize_to_prefer_buffer_size(zxy_buffer_manager_t* buffer_manager, int32_t prefer_size);

/**
 * Check whatever buffer should resize or not.
 * \param buffer_manager Pointer to the buffer manager.
 * \return Return 1 when buffer should resize.
 */
int zxy_should_resize_buffer(zxy_buffer_manager_t* buffer_manager);

/**
 * Free the allocated resources in buffer manager.
 * \param buffer_manager Pointer to the buffer manager.
 */
void zxy_free_buffer_manager(zxy_buffer_manager_t* buffer_manager);

// inline int zxy_get_base_of_buffer(zxy_buffer_manager_t* buffer_manager);

// inline int zxy_get_tail_of_buffer(zxy_buffer_manager_t* buffer_manager);

/**
 * Check if it is possible write N bytes in buffer or not.
 * \param buffer_manager Pointer to the buffer manager.
 * \param nbytes Number of bytes that willing to write.
 *\return Return 1 if it is possible to write n bytes.
 */
int zxy_can_write_nbytes_to_buffer(zxy_buffer_manager_t* buffer_manager, int32_t nbytes);

/**
 * Advance underlying pointer in the buffer manager.
 * \param buffer_manager Pointer to the buffer manager.
 * \param nbytes Number of written bytes inside buffer manager.
 */
void zxy_nbyte_written_to_buffer(zxy_buffer_manager_t* buffer_manager, int32_t nbytes);

/**
 * Retreat underlying pointer in the buffer manager.
 * \param buffer_manager Pointer to the buffer manager.
 * \param nbytes Number of read bytes.
 */
void zxy_nbyte_readed_from_buffer(zxy_buffer_manager_t* buffer_manager, int32_t nbytes);

/**
 * Clean desire number of bytes from the buffer manager.
 * \param buffer_manager Pointer to the buffer manager.
 * \param nbytes Number of bytes to clean.
 */
void zxy_clean_nbytes_from_buffer(zxy_buffer_manager_t* buffer_manager, int32_t nbytes);

#endif /* BUFFER_MANAGER_H */

