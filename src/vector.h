// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_VECTOR_H_
#define TINYC_VECTOR_H_

#include <stddef.h>

struct vector {
    void *ptr;    // Pointer to allocated memory.
    size_t cap;   // How many element this vector can hold.
    size_t len;   // How many element this vector is holding.
    size_t size;  // The size of each element in byte.
};

// Initialize `vec`. Should be called before use it.
void vector_init(struct vector *vec, size_t size);

// Push a element by copying it passed by `ptr`.
void vector_push(struct vector *vec, void *ptr);

// Append `src` to `dst`.
void vector_append(struct vector *dst, const struct vector *src);

// Pop a element from `vec`.
// If `vec` is empty, cause error.
void vector_pop(struct vector *vec);

// Peek a element in top of `vec`.
// If `vec` is empty, cause error.
void *vector_top(const struct vector *vec);

// Get `n`-th element from `vec`.
// If `n` exceed length of `vec`, cause error.
void *vector_at(const struct vector *vec, size_t n);

#endif  // TINYC_VECTOR_H_
