#include "vector.h"

#include <stdlib.h>
#include <string.h>

#include "panic.h"

#define INIT_CAP 100

static void extend(struct vector *vec, size_t atleast) {
    if (vec->cap + vec->cap < atleast) vec->cap = atleast;
    vec->cap += vec->cap;
    vec->ptr = realloc(vec->ptr, vec->cap);
    if (!vec->ptr) fatal_error("memory reallocation failed");
}

static void *ptr_at(const struct vector *vec, size_t n) {
    return (char *)vec->ptr + vec->size * n;
}

void vector_init(struct vector *vec, size_t size) {
    vec->ptr = malloc(size * INIT_CAP);
    if (!vec->ptr) fatal_error("memory allocation failed");
    vec->cap = INIT_CAP;
    vec->len = 0;
    vec->size = size;
}

void vector_push(struct vector *vec, void *ptr) {
    if (vec->len + 1 > vec->cap) extend(vec, vec->len + 1);
    memcpy(ptr_at(vec, vec->len++), ptr, vec->size);
}

void vector_append(struct vector *dst, const struct vector *src) {
    if (dst->size != src->size) fatal_error("vector size mismatched");
    if (dst->len + src->len >= dst->cap) extend(dst, dst->len + src->len);
    memcpy(ptr_at(dst, dst->len), src->ptr, src->len * src->size);
    dst->len += src->len;
}

void vector_pop(struct vector *vec) {
    if (vec->len == 0) fatal_error("empty vector");
    vec->len--;
}

void *vector_top(const struct vector *vec) {
    if (vec->len == 0) fatal_error("empty vector");
    return ptr_at(vec, vec->len - 1);
}

void *vector_at(const struct vector *vec, size_t n) {
    if (n >= vec->len) fatal_error("index exceed");
    return ptr_at(vec, n);
}
