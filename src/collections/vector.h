#ifndef TINYC_COLLECTIONS_VECTOR_H_
#define TINYC_COLLECTIONS_VECTOR_H_

#include <stdlib.h>

// A extensible type of array.
typedef struct {
    void **buf;
    size_t cap;
    size_t len;
} vector_t;

// Construct an empty vector.
// `size` must be the size of element which this vector will hold.
vector_t *vector_new();

// Extend this vector so that extra `size` element this vector can hold.
void vector_extend(vector_t *vector, size_t size);

// Push the value inside `ptr` by just storing the given pointer.
// The pointer must live long enough to the vector.
void vector_push(vector_t *vector, void *ptr);

// Append a vector `src` to `dst`.
// `src` and `dst` must have same kind of pointer.
void vector_append(vector_t *dst, const vector_t *src);

// Pop a element from vector.
void *vector_pop(vector_t *vector);

// Get the element in specified index.
void *vector_at(const vector_t *vector, size_t n);

// Peek a top element in vector.
void *vector_top(const vector_t *vector);

#endif  // TINYC_COLLECTIONS_VECTOR_H_
