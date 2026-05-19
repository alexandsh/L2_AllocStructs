#include "sys_alloc.h"
#include "allocator.h"
#include <stdlib.h>

void *sys_alloc_impl(IAllocator *self, size_t size) {
  (void)self;
  return malloc(size);
}

void sys_free_impl(IAllocator *self, void *ptr) {
  (void)self;
  free(ptr);
}

void *sys_realloc_impl(IAllocator *self, void *ptr, size_t new_size) {
  (void)self;
  return realloc(ptr, new_size);
}

void sys_reset_impl(IAllocator *self) { (void)self; }

IAllocator create_sys_alloc(void) {
  IAllocator alloc = {0};
  alloc.alloc = sys_alloc_impl;
  alloc.free = sys_free_impl;
  alloc.realloc = sys_realloc_impl;
  alloc.reset = sys_reset_impl;
  alloc.ctx = NULL;
  return alloc;
}
