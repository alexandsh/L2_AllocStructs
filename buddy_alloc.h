#ifndef BUDDY_ALLOC_H
#define BUDDY_ALLOC_H

#include <stddef.h>
#include "allocator.h"

#define BUDDY_MAX_LEVELS 32

typedef struct BuddyNode {
    struct BuddyNode* next;
} BuddyNode;

typedef struct {
    unsigned char* buffer;
    size_t total_size;
    size_t min_block;
    int max_level;
    BuddyNode* free_lists[BUDDY_MAX_LEVELS];
    unsigned char* alloc_levels;
    size_t alloc_levels_count;
} BuddyCtx;

IAllocator create_buddy_alloc(BuddyCtx* ctx, void* buffer, size_t total_size, size_t min_block);
void destroy_buddy_alloc(BuddyCtx* ctx);

#endif
