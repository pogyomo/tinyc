#ifndef TINYC_COLLECTIONS_VECTOR_H_
#define TINYC_COLLECTIONS_VECTOR_H_

#include <stddef.h>

#define VECTOR(type) vector_t
#define VECTOR_REF(type) vector_ref_t

// A generic vector.
typedef struct {
    void *buf;
    size_t cap;
    size_t len;
    size_t size;
} vector_t;

// Reference to `vector_t`
typedef vector_t *vector_ref_t;

// Initialize `vector` to be ready to use.
// `size` must be the value where `sizeof(type)` returns.
void vector_init(vector_t *vector, size_t size);

// Push value inside of `ptr` to the tail of `vector`.
void vector_push(vector_t *vector, void *ptr);

// Append `src` to `dst`.
void vector_append(vector_t *dst, const vector_t *src);

// Remove a value from tail of `vector`.
void vector_pop(vector_t *vector);

// Returns reference to the `n`-th element of `vector`.
void *vector_at(const vector_t *vector, size_t n);

// Returns reference to the last tail element of `vector`.
void *vector_top(const vector_t *vector);

// Sort `vector` with provided comparision functoin.
void vector_sort(vector_t *vector, int (*cmp)(const void *, const void *));

#endif  // TINYC_COLLECTIONS_VECTOR_H_
