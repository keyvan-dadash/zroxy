#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <sys/types.h>

typedef struct
{
    char *buffer;
    int max_size_of_buffer;

    /**
     * buffer ptr which is represent where should we start writing to buffer
     */ 
    int current_buffer_ptr;
    
    int32_t base_ptr;
    int32_t tail_ptr;
} zxy_buffer_manager_t;

zxy_buffer_manager_t* zxy_malloc_buffer_manager_with_buffer_size(int max_buffer_size);

int zxy_double_buffer_size(zxy_buffer_manager_t*);

int zxy_resize_to_prefer_buffer_size(zxy_buffer_manager_t*, int32_t);

int zxy_should_resize_buffer(zxy_buffer_manager_t*);

void zxy_free_buffer_manager(zxy_buffer_manager_t*);

// inline int zxy_get_base_of_buffer(zxy_buffer_manager_t*);

// inline int zxy_get_tail_of_buffer(zxy_buffer_manager_t*);

int zxy_can_write_nbytes_to_buffer(zxy_buffer_manager_t*, int32_t);

void zxy_nbyte_written_to_buffer(zxy_buffer_manager_t*, int32_t);

void zxy_nbyte_readed_from_buffer(zxy_buffer_manager_t*, int32_t);

void zxy_clean_nbytes_from_buffer(zxy_buffer_manager_t*, int32_t);

#endif /* BUFFER_MANAGER_H */

