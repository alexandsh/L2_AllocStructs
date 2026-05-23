#include "array_list.h"
#include "helpers.h"
#include <string.h>

void array_list_init(ArrayList *list, IAllocator *alloc,
                     size_t inital_capacity) {
  list->alloc = alloc;
  list->size = 0;
  list->capacity = inital_capacity > 0 ? inital_capacity : 4;
  list->data = (void **)i_alloc(alloc, list->capacity * sizeof(void *));
}

int array_list_add(ArrayList *list, void *item) {
  if (list->size == list->capacity) {
    size_t new_capacity = list->capacity * 2;
    size_t new_size_bytes = new_capacity * sizeof(void *);
    size_t old_size_bytes = list->capacity * sizeof(void *);

    void **new_data =
        (void **)i_realloc(list->alloc, list->data, new_size_bytes);

    if (!new_data) {
      new_data = (void **)i_alloc(list->alloc, new_size_bytes);
      if (!new_data)
        return 0;

      if (list->data) {
        memcpy(new_data, list->data, old_size_bytes);
        i_free(list->alloc, list->data);
      }
    }

    list->data = new_data;
    list->capacity = new_capacity;
  }
  list->data[list->size++] = item;
  return 1;
}

void *array_list_get(ArrayList *list, size_t index) {
  if (index >= list->size)
    return NULL;
  return list->data[index];
}

void array_list_destroy(ArrayList *list) {
  if (list->data) {
    i_free(list->alloc, list->data);
    list->data = NULL;
  }
  list->size = 0;
  list->capacity = 0;
}
