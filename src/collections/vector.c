#include "vector.h"

#include <stdlib.h>

#include "../panic.h"

vector_t *vector_new() {
    vector_t *vector = malloc(sizeof(vector_t));
    if (!vector) panic_internal("failed to allocate memory");

    vector->len = 0;
    vector->cap = 100;
    vector->buf = malloc(sizeof(void *) * vector->cap);
    if (!vector->buf) panic_internal("failed to allocate memory");
    return vector;
}

void vector_extend(vector_t *vector, size_t size) {
    vector->cap += size;
    vector->buf = malloc(sizeof(void *) * vector->cap);
    if (!vector->buf) panic_internal("failed to extend vector");
}

void vector_push(vector_t *vector, void *ptr) {
    if (vector->len >= vector->cap) {
        vector_extend(vector, 100);
    }
    vector->buf[vector->len++] = ptr;
}

void *vector_pop(vector_t *vector) {
    if (vector->len == 0) panic_internal("pop from empty vector");
    return vector->buf[--vector->len];
}

void *vector_at(vector_t *vector, size_t n) {
    if (n >= vector->len) panic_internal("at called with exceed index");
    return vector->buf[n];
}

void *vector_top(vector_t *vector) {
    if (vector->len == 0) panic_internal("top from empty vector");
    return vector->buf[vector->len - 1];
}
