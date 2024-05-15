#ifndef TINYC_PREPROCESSOR_TABLE_H_
#define TINYC_PREPROCESSOR_TABLE_H_

#include "../collections/vector.h"
#include "macro.h"

// A table holding macros and provide functions to fild macro from table.
typedef struct {
    vector_t *macros;
} mtable_t;

// Construct a new empty macro table.
mtable_t *mtable_new();

// Find a macro associated with `name`. If no such macro exist, return NULL.
macro_t *mtable_find_macro(mtable_t *table, char *name);

#endif  // TINYC_PREPROCESSOR_TABLE_H_
