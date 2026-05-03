#ifndef STUB_H
#define STUB_H

#include <cstddef>

#include "allocator.h"


void stub_free(IAllocator* self, void* ptr);
void* stub_realloc(IAllocator* self, void* ptr, size_t size);
void stub_reset(IAllocator* self);

#endif
