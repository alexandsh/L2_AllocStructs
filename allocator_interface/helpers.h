#ifndef HELPERS_H
#define HELPERS_H

#include "allocator.h"

static inline void *i_alloc(IAllocator *a, size_t size) {
  return a->alloc(a, size);
}

static inline void i_free(IAllocator *a, void *ptr) { a->free(a, ptr); }

static inline void *i_realloc(IAllocator *a, void *ptr, size_t new_size) {
  return a->realloc(a, ptr, new_size);
}

static inline void i_reset(IAllocator *a) { a->reset(a); }

#endif
