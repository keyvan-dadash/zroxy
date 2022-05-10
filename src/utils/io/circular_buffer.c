#include <stdlib.h>
#include <memory.h>

#include <sys/types.h>


#include "utils/io/circular_buffer.h"


void zxy_advance_pointer(zxy_circular_buffer_t *buffer)
{

    if (buffer->is_full) {
        if (++(buffer->tail) == buffer->maximum_size_of_buffer) {

            buffer->tail = 0;
        }
    }

    if (++(buffer->head) == buffer->maximum_size_of_buffer) {
        buffer->head = 0;
    }

    buffer->is_full = buffer->tail == buffer->head ? 1 : 0;
}

void zxy_retreat_pointer(zxy_circular_buffer_t *buffer)
{
    buffer->is_full = 0;
    if (++(buffer->tail) == buffer->maximum_size_of_buffer) {
        buffer->tail = 0;
    }
}

zxy_circular_buffer_t *zxy_create_circular_buffer(int32_t size_of_buffer)
{

    zxy_circular_buffer_t *circular_buffer = (zxy_circular_buffer_t*)calloc(1, sizeof(zxy_circular_buffer_t));

    circular_buffer->buf = (char*)malloc(size_of_buffer * sizeof(char));
    circular_buffer->head = 0;
    circular_buffer->tail = 0;
    circular_buffer->is_full = 0;
    circular_buffer->maximum_size_of_buffer = size_of_buffer;


    return circular_buffer;
}

void zxy_double_size_of_circular_buffer(zxy_circular_buffer_t *circular_buff)
{

    char *doubled_buf = (char*)malloc(circular_buff->maximum_size_of_buffer * 2 * sizeof(char));

    if (circular_buff->head > circular_buff->tail) {

        //TODO: what the fuck?? is this minus right
        memmove(doubled_buf, circular_buff->buf + circular_buff->tail, circular_buff->head - circular_buff->tail);

        circular_buff->tail = 0;
        circular_buff->head = circular_buff->head - circular_buff->tail;

    } else {
        memmove(doubled_buf, circular_buff->buf + circular_buff->tail, circular_buff->maximum_size_of_buffer);
        memmove(doubled_buf + (circular_buff->maximum_size_of_buffer - circular_buff->tail),
                    circular_buff->buf, circular_buff->head);

        circular_buff->tail = 0;
        circular_buff->head = circular_buff->maximum_size_of_buffer + circular_buff->head - circular_buff->tail;
    }

    free(circular_buff->buf);
    circular_buff->buf = doubled_buf;
    circular_buff->maximum_size_of_buffer *= 2;
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
    for (size_t i = 0; i < len; i++) {
        circular_buff->buf[circular_buff->head] = buf[i];
        zxy_advance_pointer(circular_buff);
    }
    
    return len;
}

int8_t zxy_get_buffer_from_circular_buffer(zxy_circular_buffer_t *circular_buff, char *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        data[i] = circular_buff->buf[circular_buff->tail];
        zxy_retreat_pointer(circular_buff);
    }

    return len;
}

int8_t zxy_is_circular_buffer_empty(zxy_circular_buffer_t *circular_buff)
{
    return (circular_buff->head == circular_buff->tail);
}

int32_t zxy_current_circular_buffer_size(zxy_circular_buffer_t *circular_buff)
{
    size_t size = circular_buff->maximum_size_of_buffer;

    if (!circular_buff->is_full) {

        size = (circular_buff->head >= circular_buff->tail) ? 
            (circular_buff->head - circular_buff->tail) : 
            (circular_buff->maximum_size_of_buffer + circular_buff->head - circular_buff->tail); 
    }

    return size;
}
