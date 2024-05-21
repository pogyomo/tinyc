#include "vector.h"

#include <stdlib.h>
#include <string.h>

#include "../panic.h"

#define VECTOR_INIT_CAP 100
#define VECTOR_EXTEND_SIZE 100

// Extend `vector` so that extra `size` value the vector can hold.
static void extend(vector_t *vector, size_t size) {
    vector->cap += size * vector->size;
    vector->buf = realloc(vector->buf, vector->cap * vector->size);
    if (!vector->buf) panic_internal("failed extend vector");
}

// Returns pointer where n-th value will be stored.
static inline void *ptr_at(const vector_t *vector, size_t n) {
    return (char *)vector->buf + vector->size * n;
}

void vector_init(vector_t *vector, size_t size) {
    vector->size = size;
    vector->len = 0;
    vector->cap = VECTOR_INIT_CAP;
    vector->buf = malloc(vector->size * vector->cap);
    if (!vector->buf) panic_internal("failed to allocate memory");
}

void vector_push(vector_t *vector, void *ptr) {
    if (vector->len + 1 >= vector->cap) extend(vector, 1 + VECTOR_EXTEND_SIZE);
    memcpy(ptr_at(vector, vector->len++), ptr, vector->size);
}

void vector_append(vector_t *dst, const vector_t *src) {
    if (dst->size != src->size)
        panic_internal("appending different type of vector");
    if (dst->len + src->len >= dst->cap)
        extend(dst, src->len + VECTOR_EXTEND_SIZE);
    memcpy(ptr_at(dst, dst->len), src->buf, src->len * src->size);
    dst->len += src->len;
}

void vector_pop(vector_t *vector) {
    if (vector->len == 0) panic_internal("pop from empty vector");
    vector->len--;
}

void *vector_at(const vector_t *vector, size_t n) {
    if (n >= vector->len) panic_internal("out of range indexing to vector");
    return ptr_at(vector, n);
}

void *vector_top(const vector_t *vector) {
    if (vector->len == 0) panic_internal("top from empty vector");
    return ptr_at(vector, vector->len - 1);
}
