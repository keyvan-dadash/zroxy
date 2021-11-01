

#include <stdlib.h>
#include <memory.h>

#include <sys/types.h>


#include "utils/io/circular_buffer.h"



zxy_circular_buffer_t *zxy_create_circular_buffer(int32_t size_of_buffer)
{

    zxy_circular_buffer_t *circular_buffer = (zxy_circular_buffer_t*)calloc(1, sizeof(zxy_circular_buffer_t));

    circular_buffer->buf = (char*)calloc(size_of_buffer, sizeof(char));
    circular_buffer->head = 0;
    circular_buffer->tail = 0;
    circular_buffer->is_full = 0;
    circular_buffer->maximum_size_of_buffer = size_of_buffer;


    return circular_buffer;
}

int8_t zxy_clear_circular_buffer(zxy_circular_buffer_t *circular_buff)
{
    memset(circular_buff->buf, '\0', circular_buff->maximum_size_of_buffer);
    circular_buff->head = 0;
    circular_buff->tail = 0;
    circular_buff->is_full = 0;


    return 1;
}

int8_t zxy_free_circular_buffer(zxy_circular_buffer_t *circular_buff)
{
    free(circular_buff->buf);
    free(circular_buff);

    return 1;
}

int32_t zxy_write_buffer_to_circular_buffer(zxy_circular_buffer_t *circular_buff, char *buf, size_t len)
{
    return 0;
}

int8_t zxy_get_buffer_from_circulat_buffer(zxy_circular_buffer_t *circular_buff, char *data, size_t len)
{
    return 0;
}

int8_t zxy_is_circular_buffer_empty(zxy_circular_buffer_t *circular_buff)
{
    return 0;
}

int32_t zxy_current_circular_buffer_size(zxy_circular_buffer_t *circular_buff)
{
    return 0;
}