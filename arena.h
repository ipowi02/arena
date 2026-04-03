/*
 * arena.h - 1.0.2 - Public Domain
 Non-owning arena allocator implementation in C.
 
 */

#ifndef ARENA_H
#define ARENA_H

#ifndef ARENA_NO_STDLIB
#include <stdio.h>
#include <stdlib.h>
#endif // ARENA_NO_STDLIB
struct arena;
typedef struct arena arena_t;
typedef struct arena_aligned_ctx arena_aligned_ctx_t;
static inline size_t align_up(size_t n, size_t align);
int arena_init(arena_t *a, void* buffer, size_t size);
void *arena_alloc_aligned(arena_t *a, size_t size, size_t alignment);
#define arena_alloc(Arena, T, N) arena_alloc_aligned((Arena), sizeof(T) * (N), _Alignof(T))
int arena_drop(arena_t *a);
#ifdef ARENA_IMPLEMENTATION
struct arena {
     size_t size;
     size_t pos;
     unsigned char *buf;
};
struct arena_aligned_ctx {
    arena_t *a;
    size_t alignment;
};
static inline size_t align_up(size_t n, size_t align) {
     // rounds n up to the nearest multiple of align
     return (n + align - 1) & ~(align - 1);
}

int arena_init(arena_t *a, void* buffer, size_t size) {
     // does not own memory because of the stack frame would fuck it up
     if (buffer == NULL || a == NULL) return -1;
     a->buf = buffer;
     a->size = size;
     a->pos = 0;
     return 0;
}
void *arena_alloc_aligned(arena_t *a, size_t size, size_t alignment) {
     if (a == NULL) return NULL;
     size_t aligned = align_up(a->pos, alignment);
     if (aligned + size > a->size) return NULL; // Out of memory
     a->pos = aligned + size;
     return a->buf + aligned;
}
int arena_drop(arena_t *a) {
     if (a == NULL) return -1;
     a->pos = 0;
     a->size = 0;
     a->buf = NULL;
     return 0;
}

int arena_free_wrapper(void *a, void *ctx) {
     (void)ctx;
     return arena_drop(a);
}
void *arena_alloc_wrapper(size_t size, void *ctx) {
     arena_aligned_ctx_t *c = ctx;
     return arena_alloc_aligned(c->a, size, c->alignment);
}

#endif // ARENA_IMPLEMENTATION
#endif // ARENA_H
/*
 * 1.0.1 - Added struct "arena_alignment_ctx" so arena_alloc_aligned can be used in an allocator interface.
 * 1.0.2 - Added wrapper functions for allocator interfaces.
 */
