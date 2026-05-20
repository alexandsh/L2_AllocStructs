#include "buddy_alloc.h"
#include <stdlib.h>
#include <string.h>

static int is_pow2(size_t x) {
    return x && ((x & (x - 1)) == 0);
}

static size_t pow2_floor(size_t x) {
    size_t r = 1;
    while ((r << 1) && (r << 1) <= x) r <<= 1;
    return r;
}

static int ilog2(size_t x) {
    int r = 0;
    while (x > 1) { x >>= 1; r++; }
    return r;
}

static int level_for(size_t need, size_t min_block) {
    if (need < min_block) need = min_block;
    size_t s = min_block;
    int lvl = 0;
    while (s < need) { s <<= 1; lvl++; }
    return lvl;
}

static int unlink_node(BuddyCtx* c, int lvl, BuddyNode* target) {
    BuddyNode** p = &c->free_lists[lvl];
    while (*p) {
        if (*p == target) {
            *p = target->next;
            return 1;
        }
        p = &(*p)->next;
    }
    return 0;
}

static void* buddy_alloc_impl(IAllocator* self, size_t size) {
    BuddyCtx* c = (BuddyCtx*)self->ctx;
    if (size == 0) return NULL;

    int want = level_for(size, c->min_block);
    if (want > c->max_level) return NULL;

    int lvl = want;
    while (lvl <= c->max_level && c->free_lists[lvl] == NULL) lvl++;
    if (lvl > c->max_level) return NULL;

    BuddyNode* node = c->free_lists[lvl];
    c->free_lists[lvl] = node->next;

    while (lvl > want) {
        lvl--;
        size_t half = c->min_block << lvl;
        BuddyNode* buddy = (BuddyNode*)((unsigned char*)node + half);
        buddy->next = c->free_lists[lvl];
        c->free_lists[lvl] = buddy;
    }

    size_t off = (unsigned char*)node - c->buffer;
    c->alloc_levels[off / c->min_block] = (unsigned char)(want + 1);
    return node;
}

static void buddy_free_impl(IAllocator* self, void* ptr) {
    if (ptr == NULL) return;
    BuddyCtx* c = (BuddyCtx*)self->ctx;

    unsigned char* p = (unsigned char*)ptr;
    if (p < c->buffer || p >= c->buffer + c->total_size) return;

    size_t off = p - c->buffer;
    size_t idx = off / c->min_block;
    int lvl = (int)c->alloc_levels[idx] - 1;
    if (lvl < 0) return;
    c->alloc_levels[idx] = 0;

    while (lvl < c->max_level) {
        size_t bs = c->min_block << lvl;
        size_t boff = off ^ bs;
        BuddyNode* buddy = (BuddyNode*)(c->buffer + boff);
        if (!unlink_node(c, lvl, buddy)) break;
        if (boff < off) off = boff;
        lvl++;
    }

    BuddyNode* n = (BuddyNode*)(c->buffer + off);
    n->next = c->free_lists[lvl];
    c->free_lists[lvl] = n;
}

static void* buddy_realloc_impl(IAllocator* self, void* ptr, size_t new_size) {
    BuddyCtx* c = (BuddyCtx*)self->ctx;
    if (ptr == NULL) return buddy_alloc_impl(self, new_size);
    if (new_size == 0) { buddy_free_impl(self, ptr); return NULL; }

    size_t off = (unsigned char*)ptr - c->buffer;
    size_t idx = off / c->min_block;
    int lvl = (int)c->alloc_levels[idx] - 1;
    if (lvl < 0) return NULL;

    size_t cur = c->min_block << lvl;
    if (new_size <= cur) return ptr;

    void* np = buddy_alloc_impl(self, new_size);
    if (np) {
        memcpy(np, ptr, cur);
        buddy_free_impl(self, ptr);
    }
    return np;
}

static void buddy_reset_impl(IAllocator* self) {
    BuddyCtx* c = (BuddyCtx*)self->ctx;
    for (int i = 0; i < BUDDY_MAX_LEVELS; ++i) c->free_lists[i] = NULL;
    memset(c->alloc_levels, 0, c->alloc_levels_count);
    BuddyNode* n = (BuddyNode*)c->buffer;
    n->next = NULL;
    c->free_lists[c->max_level] = n;
}

IAllocator create_buddy_alloc(BuddyCtx* ctx, void* buffer, size_t total_size, size_t min_block) {
    if (!is_pow2(total_size)) total_size = pow2_floor(total_size);
    if (min_block < sizeof(BuddyNode)) min_block = sizeof(BuddyNode);
    if (!is_pow2(min_block)) min_block = pow2_floor(min_block);

    ctx->buffer = (unsigned char*)buffer;
    ctx->total_size = total_size;
    ctx->min_block = min_block;
    ctx->max_level = ilog2(total_size / min_block);

    for (int i = 0; i < BUDDY_MAX_LEVELS; ++i) ctx->free_lists[i] = NULL;

    ctx->alloc_levels_count = total_size / min_block;
    ctx->alloc_levels = (unsigned char*)calloc(ctx->alloc_levels_count, 1);

    BuddyNode* n = (BuddyNode*)buffer;
    n->next = NULL;
    ctx->free_lists[ctx->max_level] = n;

    IAllocator a;
    a.alloc   = buddy_alloc_impl;
    a.free    = buddy_free_impl;
    a.realloc = buddy_realloc_impl;
    a.reset   = buddy_reset_impl;
    a.ctx     = ctx;
    return a;
}

void destroy_buddy_alloc(BuddyCtx* ctx) {
    free(ctx->alloc_levels);
    ctx->alloc_levels = NULL;
    ctx->alloc_levels_count = 0;
}
