#ifndef TINYC_PREPROCESSOR_MACRO_H_
#define TINYC_PREPROCESSOR_MACRO_H_

#include "../collections/dict.h"
#include "../collections/vector.h"

// All possible macros in program.
typedef struct {
    enum {
        MACRO_OBJECT,
        MACRO_FUNCTION,
    } kind;
    vector_t body;    // Vector with `token_t`.
    vector_t params;  // Vector with `string_t`. Empty if kind == MACRO_OBJECT.
} macro_t;

// Initialize `macro` as object macro.
void macro_object(macro_t *macro, vector_t body);

// Initialize `macro` as function-like macro.
void macro_function(macro_t *macro, vector_t body, vector_t params);

// A collection of macros.
typedef struct {
    dict_t table;  // Dictionary where value is `macro_t`.
} mtable_t;

// Initialize `mtable` to be empty.
void mtable_init(mtable_t *mtable);

// Insert macro with its name.
void mtable_insert(mtable_t *mtable, const char *name, macro_t macro);

// Remove macro where name is `name`.
void mtable_remove(mtable_t *mtable, const char *name);

// Find a macro from `mtable` where its name is `name`.
// Returns NULL if no such macro exist.
macro_t *mtable_find(mtable_t *mtable, const char *name);

#endif  // TINYC_PREPROCESSOR_MACRO_H_
