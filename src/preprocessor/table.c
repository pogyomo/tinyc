#include "table.h"

#include <stdlib.h>
#include <string.h>

#include "../panic.h"
#include "macro.h"

mtable_t *mtable_new() {
    mtable_t *table = malloc(sizeof(mtable_t));
    if (!table) panic_internal("faild to allocate memory");
    table->macros = vector_new();
    return table;
}

macro_t *mtable_find_macro(mtable_t *table, char *name) {
    for (int i = 0; i < table->macros->len; i++) {
        macro_t *macro = vector_at(table->macros, i);
        if (strcmp(macro->name->str, name) == 0) {
            return macro;
        }
    }
    return NULL;
}
