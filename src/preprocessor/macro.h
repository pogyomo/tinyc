#ifndef TINYC_PREPROCESSOR_MACRO_H_
#define TINYC_PREPROCESSOR_MACRO_H_

#include "../collections/string.h"
#include "../collections/vector.h"

typedef enum {
    MACRO_OBJECT,
    MACRO_FUNCTION,
} macro_kind_t;

typedef struct {
    macro_kind_t kind;
    string_t *name;
    vector_t *body;    // Body of this macro. Holding token_t
    vector_t *params;  // Parameter of this macro, or empty
                       // if kind != MACRO_FUNCTION. This holding string_t.
} macro_t;

// Construct a new object macro.
macro_t *macro_new_object(string_t *name, vector_t *body);

// Construct a new function-like macro.
macro_t *macro_new_function(string_t *name, vector_t *body, vector_t *params);

#endif  // TINYC_PREPROCESSOR_MACRO_H_
