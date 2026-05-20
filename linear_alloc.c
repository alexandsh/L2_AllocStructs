#include "linear_alloc.h"
#include <string.h>

static void* lin_alloc(IAllocator* self, size_t size) {
    LinearCtx* c = (LinearCtx*)self->ctx;
    size_t n = align_up(size);
    if (c->offset + n > c->size)
        return NULL;
    void* p = c->buffer + c->offset;
    c->last_offset = c->offset;
    c->last_size = n;
    c->offset += n;
    return p;
}

static void* lin_realloc(IAllocator* self, void* ptr, size_t new_size) {
    LinearCtx* c = (LinearCtx*)self->ctx;
    if (ptr == NULL)
        return lin_alloc(self, new_size);

    size_t n = align_up(new_size);

    if ((unsigned char*)ptr == c->buffer + c->last_offset) {
        if (c->last_offset + n > c->size)
            return NULL;
        c->offset = c->last_offset + n;
        c->last_size = n;
        return ptr;
    }

    void* np = lin_alloc(self, new_size);
    if (np && new_size > 0)
        memcpy(np, ptr, new_size);
    return np;
}

static void lin_reset(IAllocator* self) {
    LinearCtx* c = (LinearCtx*)self->ctx;
    c->offset = 0;
    c->last_offset = 0;
    c->last_size = 0;
}

IAllocator create_linear_alloc(LinearCtx* ctx, void* buffer, size_t size) {
    ctx->buffer = (unsigned char*)buffer;
    ctx->size = size;
    ctx->offset = 0;
    ctx->last_offset = 0;
    ctx->last_size = 0;

    IAllocator a;
    a.alloc   = lin_alloc;
    a.free    = stub_free;
    a.realloc = lin_realloc;
    a.reset   = lin_reset;
    a.ctx     = ctx;
    return a;
}
