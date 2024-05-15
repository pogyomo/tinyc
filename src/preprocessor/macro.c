#include "macro.h"

#include <stdlib.h>

#include "../panic.h"

macro_t *macro_new_object(string_t *name, vector_t *body) {
    macro_t *macro = malloc(sizeof(macro_t));
    if (!macro) panic_internal("failed to allocate memory");
    macro->kind = MACRO_OBJECT;
    macro->name = name;
    macro->body = body;
    macro->params = vector_new();
    return macro;
}

macro_t *macro_new_function(string_t *name, vector_t *body, vector_t *params) {
    macro_t *macro = malloc(sizeof(macro_t));
    if (!macro) panic_internal("failed to allocate memory");
    macro->kind = MACRO_FUNCTION;
    macro->name = name;
    macro->body = body;
    macro->params = params;
    return macro;
}
