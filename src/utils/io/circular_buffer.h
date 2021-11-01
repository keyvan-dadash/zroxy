

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <sys/types.h>

typedef struct
{
    
    char *buf;

    /**
     * info of buffer
     */
    size_t tail;
    size_t head;
    size_t maximum_size_of_buffer;

    /**
     * extra info
     */
    int8_t is_full;


} zxy_circular_buffer_t;


zxy_circular_buffer_t *zxy_create_circular_buffer(int32_t size_of_buffer);

int8_t zxy_clear_circular_buffer(zxy_circular_buffer_t *circular_buff);

int8_t zxy_free_circular_buffer(zxy_circular_buffer_t *circular_buff);

int32_t zxy_write_buffer_to_circular_buffer(zxy_circular_buffer_t *circular_buff, char *buf, size_t len);

int8_t zxy_get_buffer_from_circulat_buffer(zxy_circular_buffer_t *circular_buff, char *data, size_t len);

int8_t zxy_is_circular_buffer_empty(zxy_circular_buffer_t *circular_buff);

int32_t zxy_current_circular_buffer_size(zxy_circular_buffer_t *circular_buff);



#endif /* CIRCULAR_BUFFER_H */