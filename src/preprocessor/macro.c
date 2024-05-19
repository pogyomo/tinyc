#include "macro.h"

#include "../collections/string.h"

void macro_object(macro_t *macro, vector_t body) {
    macro->kind = MACRO_OBJECT;
    macro->body = body;
    vector_init(&macro->params, sizeof(string_t));
}

void macro_function(macro_t *macro, vector_t body, vector_t params) {
    macro->kind = MACRO_FUNCTION;
    macro->body = body;
    macro->params = params;
}

void mtable_init(mtable_t *mtable) {
    dict_init(&mtable->table, sizeof(macro_t));
}

void mtable_insert(mtable_t *mtable, const char *name, macro_t macro) {
    dict_insert(&mtable->table, name, &macro);
}

void mtable_remove(mtable_t *mtable, const char *name) {
    dict_remove(&mtable->table, name);
}

macro_t *mtable_find(mtable_t *mtable, const char *name) {
    return dict_find(&mtable->table, name);
}
