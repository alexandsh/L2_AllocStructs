#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

typedef struct IAllocator {
    void* (*alloc)(struct IAllocator* self, size_t size);
    void  (*free)(struct IAllocator* self, void* ptr);
    void* (*realloc)(struct IAllocator* self, void* ptr, size_t new_size);
    void  (*reset)(struct IAllocator* self);
    void* ctx;
} IAllocator;

#define ALLOC_ALIGN 8u

static inline size_t align_up(size_t s) {
    return (s + (ALLOC_ALIGN - 1)) & ~((size_t)(ALLOC_ALIGN - 1));
}

void  stub_free(IAllocator* self, void* ptr);
void* stub_realloc(IAllocator* self, void* ptr, size_t new_size);
void  stub_reset(IAllocator* self);

IAllocator create_sys_alloc(void);

static inline void* i_alloc(IAllocator* a, size_t sz)            { return a->alloc(a, sz); }
static inline void  i_free(IAllocator* a, void* p)               { a->free(a, p); }
static inline void* i_realloc(IAllocator* a, void* p, size_t sz) { return a->realloc(a, p, sz); }
static inline void  i_reset(IAllocator* a)                       { a->reset(a); }

#endif
