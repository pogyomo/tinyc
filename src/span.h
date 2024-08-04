// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_SPAN_H_
#define TINYC_SPAN_H_

#include <stddef.h>

// Start position of a character in source code.
struct position {
    size_t row;
    size_t col;
};

// Inclusive range of source code.
struct span {
    size_t id;
    struct position start;
    struct position end;
};

// Merge `lhs` and `rhs`, then update `merged` with it.
void merge_span(const struct span *lhs, const struct span *rhs,
                struct span *merged);

#endif  // TINYC_SPAN_H_
