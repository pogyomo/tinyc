#include "fun_def.h"

#include <assert.h>
#include <stdlib.h>

#include "../../panic.h"
#include "stmt.h"

struct fun_def *fun_def_new(struct type *type, struct fun_def_name *name,
                            struct fun_def_param *params, struct stmt *body,
                            struct span *span) {
    assert(type != NULL && name != NULL && body != NULL &&
           body->kind == STMT_BLOCK && span != NULL);
    struct fun_def *res = malloc(sizeof(struct fun_def));
    if (!res) fatal_error("memory allocation failed");
    res->span = *span;
    res->type = type;
    res->name = *name;
    res->params = params;
    res->body = body;
    return res;
}

struct fun_def_param *fun_def_param_new() {
    struct fun_def_param *param = malloc(sizeof(struct fun_def_param));
    if (!param) fatal_error("memory allocation failed");
    return param;
}

struct fun_def_param *fun_def_param_normal_new(struct type *type,
                                               struct fun_def_param_name *name,
                                               struct span *span) {
    assert(type != NULL && name != NULL && span != NULL);
    struct fun_def_param *res = fun_def_param_new();
    res->next = NULL;
    res->kind = FUN_DEF_PARAM_NORMAL;
    res->span = *span;
    res->type = type;
    res->name = *name;
    return res;
}

struct fun_def_param *fun_def_param_varidic_new(struct span *span) {
    assert(span != NULL);
    struct fun_def_param *res = fun_def_param_new();
    res->next = NULL;
    res->kind = FUN_DEF_PARAM_VARIDIC;
    res->span = *span;
    return res;
}
