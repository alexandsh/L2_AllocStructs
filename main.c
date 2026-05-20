#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "linear_alloc.h"
#include "pool_alloc.h"
#include "buddy_alloc.h"

static int check(const char* name, int ok) {
    printf("  [%s] %s\n", ok ? "OK" : "FAIL", name);
    return ok;
}

static void test_linear(void) {
    printf("== Linear ==\n");
    unsigned char buf[256];
    LinearCtx ctx;
    IAllocator a = create_linear_alloc(&ctx, buf, sizeof(buf));

    void* p1 = i_alloc(&a, 10);
    void* p2 = i_alloc(&a, 30);
    void* p3 = i_alloc(&a, 7);
    check("alloc != NULL", p1 && p2 && p3);
    check("p2 > p1", (unsigned char*)p2 > (unsigned char*)p1);
    check("alignment 8", ((size_t)p2 & 7) == 0 && ((size_t)p3 & 7) == 0);

    void* big = i_alloc(&a, 1024);
    check("overflow -> NULL", big == NULL);

    void* g = i_alloc(&a, 16);
    void* g2 = i_realloc(&a, g, 64);
    check("realloc last in place", g == g2);

    i_reset(&a);
    void* after = i_alloc(&a, 8);
    check("after reset = start", after == buf);
}

static void test_pool(void) {
    printf("== Pool ==\n");
    unsigned char buf[256];
    PoolCtx ctx;
    IAllocator a = create_pool_alloc(&ctx, buf, sizeof(buf), 32);

    void* arr[8];
    int ok = 1;
    for (int i = 0; i < 8; ++i) {
        arr[i] = i_alloc(&a, 32);
        if (!arr[i]) ok = 0;
    }
    check("8 blocks of 32", ok);
    check("overflow -> NULL", i_alloc(&a, 32) == NULL);

    i_free(&a, arr[3]);
    i_free(&a, arr[5]);
    void* a1 = i_alloc(&a, 32);
    void* a2 = i_alloc(&a, 32);
    check("reuse after free", a1 && a2);

    check("too big -> NULL", i_alloc(&a, 64) == NULL);

    i_reset(&a);
    int reuse_all = 1;
    for (int i = 0; i < 8; ++i) {
        if (!i_alloc(&a, 32)) reuse_all = 0;
    }
    check("reset restores all blocks", reuse_all);
}

static void test_buddy(void) {
    printf("== Buddy ==\n");
    size_t total = 1024;
    unsigned char* buf = (unsigned char*)malloc(total);
    BuddyCtx ctx;
    IAllocator a = create_buddy_alloc(&ctx, buf, total, 32);

    void* p1 = i_alloc(&a, 60);
    void* p2 = i_alloc(&a, 100);
    void* p3 = i_alloc(&a, 30);
    check("3 allocs", p1 && p2 && p3);
    check("distinct", p1 != p2 && p2 != p3 && p1 != p3);

    i_free(&a, p1);
    i_free(&a, p3);
    i_free(&a, p2);

    void* big = i_alloc(&a, 1024);
    check("merged back to full block", big == buf);
    i_free(&a, big);

    void* r = i_alloc(&a, 30);
    memcpy(r, "hello", 6);
    void* r2 = i_realloc(&a, r, 200);
    check("realloc grow", r2 != NULL);
    check("realloc keeps data", r2 && memcmp(r2, "hello", 6) == 0);
    i_free(&a, r2);

    int filled = 0;
    void* hold[64];
    for (int i = 0; i < 64; ++i) {
        hold[i] = i_alloc(&a, 32);
        if (hold[i]) filled++;
    }
    check("fill all min blocks", filled == 32);
    for (int i = 0; i < 64; ++i) i_free(&a, hold[i]);

    destroy_buddy_alloc(&ctx);
    free(buf);
}

static void test_sys(void) {
    printf("== Sys ==\n");
    IAllocator a = create_sys_alloc();
    int* p = (int*)i_alloc(&a, sizeof(int) * 4);
    check("malloc wrap", p != NULL);
    for (int i = 0; i < 4; ++i) p[i] = i * i;
    int* q = (int*)i_realloc(&a, p, sizeof(int) * 8);
    check("realloc wrap", q != NULL);
    check("data preserved", q && q[2] == 4);
    i_free(&a, q);
}

int main(void) {
    test_sys();
    test_linear();
    test_pool();
    test_buddy();
    printf("done\n");
    return 0;
}
