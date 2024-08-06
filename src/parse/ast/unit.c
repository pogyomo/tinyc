#include "unit.h"

#include <assert.h>
#include <stdlib.h>

#include "../../panic.h"

struct trans_unit *trans_unit_new() {
    struct trans_unit *unit = malloc(sizeof(struct trans_unit));
    if (!unit) fatal_error("memory allocation failed");
    return unit;
}

struct trans_unit *tran_unit_decl_new(struct decl *decl, struct span *span) {
    assert(span != NULL);
    struct trans_unit *res = trans_unit_new();
    res->next = NULL;
    res->kind = TRANS_UNIT_DECL;
    res->span = *span;
    res->decl = decl;
    return res;
}

struct trans_unit *tran_unit_fun_def_new(struct fun_def *fun_def,
                                         struct span *span) {
    assert(fun_def != NULL && span != NULL);
    struct trans_unit *res = trans_unit_new();
    res->next = NULL;
    res->kind = TRANS_UNIT_FUN_DEF;
    res->span = *span;
    res->fun_def = fun_def;
    return res;
}
