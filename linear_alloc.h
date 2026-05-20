#ifndef LINEAR_ALLOC_H
#define LINEAR_ALLOC_H

#include <stddef.h>
#include "allocator.h"

typedef struct {
    unsigned char* buffer;
    size_t size;
    size_t offset;
    size_t last_offset;
    size_t last_size;
} LinearCtx;

IAllocator create_linear_alloc(LinearCtx* ctx, void* buffer, size_t size);

#endif
