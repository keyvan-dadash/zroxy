#include <stdlib.h>
#include <sys/types.h>
#include <memory.h>

#include "defines.h"
#include "logging/logs.h"
#include "utils/io/buffer_manager.h"

zxy_buffer_manager_t* zxy_malloc_buffer_manager_with_buffer_size(int max_buffer_size)
{
    zxy_buffer_manager_t *buffer_manager = (zxy_buffer_manager_t*)calloc(1, sizeof(zxy_buffer_manager_t));

    buffer_manager->buffer = (char*)calloc(max_buffer_size, sizeof(char));


    buffer_manager->max_size_of_buffer = max_buffer_size;
    buffer_manager->current_buffer_ptr = 0;

    return buffer_manager;
}

int zxy_double_buffer_size(zxy_buffer_manager_t* buffer_manager)
{
    LOG_INFO("new size will be %d\n", ((buffer_manager->max_size_of_buffer - 1) * 2));
    buffer_manager->buffer = (char *)realloc(buffer_manager->buffer, ((buffer_manager->max_size_of_buffer - 1) * 2) );
    memset((buffer_manager->buffer + buffer_manager->max_size_of_buffer), '\0', (buffer_manager->max_size_of_buffer - 1));
    buffer_manager->max_size_of_buffer = ((buffer_manager->max_size_of_buffer - 1) * 2 + 1);

    return buffer_manager->max_size_of_buffer;
}

int zxy_resize_to_prefer_buffer_size(zxy_buffer_manager_t* buffer_manager, int32_t prefer_size)
{
    buffer_manager->buffer = (char *)realloc(buffer_manager->buffer, prefer_size);

    if (prefer_size > buffer_manager->max_size_of_buffer) {
        memset((buffer_manager->buffer + buffer_manager->max_size_of_buffer), '\0', prefer_size - buffer_manager->max_size_of_buffer);
    }
    
    if (prefer_size < READ_BUF_SIZE) {
        buffer_manager->max_size_of_buffer = READ_BUF_SIZE;
    } else {
        buffer_manager->max_size_of_buffer = prefer_size;
    }

    return buffer_manager->max_size_of_buffer;
}


int zxy_should_resize_buffer(zxy_buffer_manager_t* buffer_manager)
{
    return buffer_manager->buffer[buffer_manager->max_size_of_buffer - 1] != '\0';
}
/*
int zxy_get_base_of_buffer(zxy_buffer_manager_t* buffer_manager)
{
  return buffer_manager->base_ptr;
}

int zxy_get_tail_of_buffer(zxy_buffer_manager_t* buffer_manager)
{
  return buffer_manager->tail_ptr;
}
*/
void zxy_free_buffer_manager(zxy_buffer_manager_t* buffer_manager)
{
    free(buffer_manager->buffer);
    free(buffer_manager);
}

int zxy_can_write_nbytes_to_buffer(zxy_buffer_manager_t* buffer_manager, int32_t nbytes)
{
    if (buffer_manager->max_size_of_buffer < buffer_manager->current_buffer_ptr + nbytes) return 0;
    else return 1;
}

void zxy_nbyte_written_to_buffer(zxy_buffer_manager_t* buffer_manager, int32_t nbytes)
{
    buffer_manager->current_buffer_ptr += nbytes;
}

void zxy_nbyte_readed_from_buffer(zxy_buffer_manager_t* buffer_manager, int32_t nbytes)
{
    buffer_manager->current_buffer_ptr -= nbytes;
}

void zxy_clean_nbytes_from_buffer(zxy_buffer_manager_t* buffer_manager, int32_t nbytes)
{
    memset(buffer_manager->buffer, '\0', nbytes);
}
