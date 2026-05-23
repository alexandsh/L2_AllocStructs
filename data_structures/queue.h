#ifndef QUEUE_H
#define QUEUE_H

#include "allocator.h"
#include <stddef.h>

typedef struct Node {
  void *data;
  struct Node *next;
} Node;

typedef struct {
  IAllocator *alloc;
  Node *head;
  Node *tail;
} Queue;

void queue_init(Queue *q, IAllocator *alloc);
int queue_push(Queue *q, void *val);
void *queue_pop(Queue *q);
int queue_is_empty(Queue *q);
void queue_destroy(Queue *q);

#endif
