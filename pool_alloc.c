#include "pool_alloc.h"

static void pool_build_freelist(PoolCtx *c) {
  c->free_list = NULL;
  for (size_t i = 0; i < c->block_count; ++i) {
    PoolNode *n = (PoolNode *)(c->buffer + i * c->block_size);
    n->next = c->free_list;
    c->free_list = n;
  }
}

static void *pool_alloc(IAllocator *self, size_t size) {
  PoolCtx *c = (PoolCtx *)self->ctx;
  if (size == 0 || size > c->block_size || c->free_list == NULL)
    return NULL;
  PoolNode *n = c->free_list;
  c->free_list = n->next;
  return n;
}

static void pool_free(IAllocator *self, void *ptr) {
  if (ptr == NULL)
    return;
  PoolCtx *c = (PoolCtx *)self->ctx;
  unsigned char *p = (unsigned char *)ptr;
  if (p < c->buffer || p >= c->buffer + c->block_count * c->block_size)
    return;
  PoolNode *n = (PoolNode *)ptr;
  n->next = c->free_list;
  c->free_list = n;
}

static void pool_reset(IAllocator *self) {
  PoolCtx *c = (PoolCtx *)self->ctx;
  pool_build_freelist(c);
}

IAllocator create_pool_alloc(PoolCtx *ctx, void *buffer, size_t buffer_size,
                             size_t block_size) {
  size_t bs = align_up(block_size);
  if (bs < sizeof(PoolNode))
    bs = align_up(sizeof(PoolNode));

  ctx->buffer = (unsigned char *)buffer;
  ctx->block_size = bs;
  ctx->block_count = buffer_size / bs;
  ctx->free_list = NULL;
  pool_build_freelist(ctx);

  IAllocator a;
  a.alloc = pool_alloc;
  a.free = pool_free;
  a.realloc = stub_realloc;
  a.reset = pool_reset;
  a.ctx = ctx;
  return a;
}
