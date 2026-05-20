#ifndef POOL_ALLOC_H
#define POOL_ALLOC_H

#include <stddef.h>
#include "allocator.h"

typedef struct PoolNode {
    struct PoolNode* next;
} PoolNode;

typedef struct {
    unsigned char* buffer;
    size_t block_size;
    size_t block_count;
    PoolNode* free_list;
} PoolCtx;

IAllocator create_pool_alloc(PoolCtx* ctx, void* buffer, size_t buffer_size, size_t block_size);

#endif
