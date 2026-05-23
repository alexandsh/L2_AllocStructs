#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include "allocator.h"
#include <stddef.h>

typedef struct {
  IAllocator *alloc;
  void **data;
  size_t size;
  size_t capacity;
} ArrayList;

void array_list_init(ArrayList *list, IAllocator *alloc,
                     size_t initial_capacity);
int array_list_add(ArrayList *list, void *item);
void *array_list_get(ArrayList *list, size_t index);
void array_list_destroy(ArrayList *list);

#endif
