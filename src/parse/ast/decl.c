#include "decl.h"

#include <assert.h>
#include <stdlib.h>

#include "../../panic.h"

struct initializer *initializer_new() {
    struct initializer *init = malloc(sizeof(struct initializer));
    if (!init) fatal_error("memory allocation failed");
    return init;
}

struct initializer *initializer_expr_new(struct expr *expr, struct span *span) {
    assert(expr != NULL && span != NULL);
    struct initializer *res = initializer_new();
    res->kind = INITIALIZER_EXPR;
    res->span = *span;
    res->expr = expr;
    return res;
}

struct initializer *initializer_list_new(struct initializer_list *list,
                                         struct span *span) {
    assert(list != NULL && span != NULL);
    struct initializer *res = initializer_new();
    res->kind = INITIALIZER_LIST;
    res->span = *span;
    res->list = *list;
    return res;
}

struct initializer_list_item *initializer_list_item_new(
    struct designator *desigs, struct initializer *value, struct span *span) {
    assert(value != NULL && span != NULL);
    struct initializer_list_item *item =
        malloc(sizeof(struct initializer_list_item));
    if (!item) fatal_error("memory allocation failed");
    item->next = NULL;
    item->span = *span;
    item->desigs = desigs;
    item->value = value;
    return item;
}

struct designator *designator_new() {
    struct designator *desig = malloc(sizeof(struct designator));
    if (!desig) fatal_error("memory allocation failed");
    return desig;
}

struct designator *designator_index_new(struct expr *index, struct span *span) {
    assert(index != NULL && span != NULL);
    struct designator *res = designator_new();
    res->next = NULL;
    res->kind = DESIGNATOR_INDEX;
    res->span = *span;
    res->index = index;
    return res;
}

struct designator *designator_ident_new(char *ident, struct span *span) {
    assert(span != NULL);
    struct designator *res = designator_new();
    res->next = NULL;
    res->kind = DESIGNATOR_IDENT;
    res->span = *span;
    res->ident = ident;
    return res;
}

struct storage_class *storage_class_new(enum storage_class_kind kind,
                                        struct span *span) {
    struct storage_class *class = malloc(sizeof(struct storage_class));
    if (!class) fatal_error("memory allocation failed");
    class->kind = kind;
    class->span = *span;
    return class;
}

struct function_spec *function_spec_new(enum function_spec_kind kind,
                                        struct span *span) {
    struct function_spec *spec = malloc(sizeof(struct storage_class));
    if (!spec) fatal_error("memory allocation failed");
    spec->kind = kind;
    spec->span = *span;
    return spec;
}

struct decl *decl_new() {
    struct decl *decl = malloc(sizeof(struct decl));
    if (!decl) fatal_error("memory allocation failed");
    return decl;
}

struct decl *decl_var_new(struct type *type, struct storage_class *class,
                          struct decl_var_name *name, struct initializer *init,
                          struct span *span) {
    assert(type != NULL && name != NULL && span != NULL);
    struct decl *res = decl_new();
    res->next = NULL;
    res->kind = DECL_VAR;
    res->span = *span;
    res->type = type;
    res->class = class;
    res->var.name = *name;
    res->var.init = init;
    return res;
}

struct decl *decl_fun_new(struct type *type, struct storage_class *class,
                          struct function_spec *func_spec,
                          struct decl_fun_name *name,
                          struct decl_fun_param *params, struct span *span) {
    assert(type != NULL && name != NULL && span != NULL);
    struct decl *res = decl_new();
    res->next = NULL;
    res->kind = DECL_FUN;
    res->span = *span;
    res->type = type;
    res->class = class;
    res->fun.func_spec = func_spec;
    res->fun.name = *name;
    res->fun.params = params;
    return res;
}

struct decl_fun_param *decl_fun_param_new() {
    struct decl_fun_param *param = malloc(sizeof(struct decl_fun_param));
    if (!param) fatal_error("memory allocation failed");
    return param;
}

struct decl_fun_param *decl_fun_param_normal_new(
    struct type *type, struct decl_fun_param_name *name, struct span *span) {
    assert(type != NULL && name != NULL && span != NULL);
    struct decl_fun_param *res = decl_fun_param_new();
    res->next = NULL;
    res->kind = DECL_FUN_PARAM_NORMAL;
    res->span = *span;
    res->type = type;
    res->name = *name;
    return res;
}

struct decl_fun_param *decl_fun_param_varidic_new(struct span *span) {
    assert(span != NULL);
    struct decl_fun_param *res = decl_fun_param_new();
    res->next = NULL;
    res->kind = DECL_FUN_PARAM_VARIDIC;
    res->span = *span;
    return res;
}
