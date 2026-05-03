#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>


typedef struct IAllocator {
    void* (*alloc)(struct IAllocator* self, size_t size);
    void (*free)(struct IAllocator* self, void* ptr);
    void* (*realloc)(struct IAllocator* self, void* ptr, size_t new_size);
    void (*reset)(struct IAllocator* self);
    void* ctx;
} IAllocator;

#endif
