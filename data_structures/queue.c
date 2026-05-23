#include "queue.h"
#include "helpers.h"

void queue_init(Queue *q, IAllocator *alloc) {
  q->alloc = alloc;
  q->head = NULL;
  q->tail = NULL;
}

int queue_push(Queue *q, void *val) {
  Node *n = (Node *)i_alloc(q->alloc, sizeof(Node));
  if (!n)
    return 0;

  n->data = val;
  n->next = NULL;

  if (q->tail == NULL) {
    q->head = n;
    q->tail = n;
  } else {
    q->tail->next = n;
    q->tail = n;
  }
  return 1;
}

void *queue_pop(Queue *q) {
  if (q->head == NULL)
    return NULL;

  Node *temp = q->head;
  void *data = temp->data;

  q->head = q->head->next;
  if (q->head == NULL) {
    q->tail = NULL;
  }

  i_free(q->alloc, temp);
  return data;
}

int queue_is_empty(Queue *q) { return q->head == NULL; }

void queue_destroy(Queue *q) {
  while (!queue_is_empty(q)) {
    queue_pop(q);
  }
}
