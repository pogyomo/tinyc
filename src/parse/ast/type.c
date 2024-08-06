#include "type.h"

#include <assert.h>
#include <malloc.h>
#include <stdlib.h>

#include "../../panic.h"

struct type *type_new() {
    struct type *type = malloc(sizeof(struct type));
    if (!type) fatal_error("memory allocation failed");
    return type;
}

struct type *type_builtin_new(enum type_builtin_kind kind, struct span *span) {
    assert(span != NULL);
    struct type *res = type_new();
    res->kind = TYPE_BUILTIN;
    res->span = *span;
    res->builtin.kind = kind;
    return res;
}

struct type *type_struct_or_union_new(bool is_struct,
                                      struct type_struct_or_union_name *name,
                                      struct type_struct_or_union_decl *decls,
                                      struct span *span) {
    assert(name != NULL && span != NULL);
    struct type *res = type_new();
    res->kind = TYPE_STRUCT_OR_UNION;
    res->span = *span;
    res->struct_or_union.is_struct = is_struct;
    res->struct_or_union.name = *name;
    res->struct_or_union.decls = decls;
    return res;
}

struct type_struct_or_union_decl *type_struct_or_union_decl_new(
    struct type *type, struct type_struct_or_union_decl_name *name,
    struct span *span, struct expr *bit_field) {
    assert(type != NULL && name != NULL && span != NULL);
    struct type_struct_or_union_decl *decl =
        malloc(sizeof(struct type_struct_or_union_decl));
    if (!decl) fatal_error("memory allocation failed");
    decl->next = NULL;
    decl->span = *span;
    decl->type = type;
    decl->name = *name;
    decl->bit_field = bit_field;
    return decl;
}

struct type *type_enum_new(struct type_enum_name *name,
                           struct type_enum_enumerator *enumerators,
                           struct span *span) {
    assert(name != NULL && span != NULL);
    struct type *res = type_new();
    res->kind = TYPE_ENUM;
    res->span = *span;
    res->enum_.name = *name;
    res->enum_.enumerators = enumerators;
    return res;
}

struct type_enum_enumerator *type_enum_enumerator_new(
    struct type_enum_enumerator_name *name, struct expr *value,
    struct span *span) {
    assert(name != NULL && span != NULL);
    struct type_enum_enumerator *enumerator =
        malloc(sizeof(struct type_enum_enumerator));
    if (!enumerator) fatal_error("memory allocation failed");
    enumerator->next = NULL;
    enumerator->span = *span;
    enumerator->name = *name;
    enumerator->value = value;
    return enumerator;
}

struct type *type_array_new(struct type *of, bool has_static,
                            struct type_quant *quants,
                            struct type_array_size *size, struct span *span) {
    assert(of != NULL && span != NULL);
    struct type *res = type_new();
    res->kind = TYPE_ARRAY;
    res->span = *span;
    res->array.of = of;
    res->array.has_static = has_static;
    res->array.quants = quants;
    res->array.size = size;
    return res;
}

struct type_array_size *type_array_size_new() {
    struct type_array_size *size = malloc(sizeof(struct type_array_size));
    if (!size) fatal_error("memory allocation failed");
    return size;
}

struct type_array_size *type_array_size_expr_new(struct expr *value,
                                                 struct span *span) {
    assert(span != NULL);
    struct type_array_size *res = type_array_size_new();
    res->kind = TYPE_ARRAY_SIZE_EXPR;
    res->span = *span;
    res->value = value;
    return res;
}

struct type_array_size *type_array_size_star_new(struct span *span) {
    assert(span != NULL);
    struct type_array_size *res = type_array_size_new();
    res->kind = TYPE_ARRAY_SIZE_STAR;
    res->span = *span;
    return res;
}

struct type *type_pointer_new(struct type *of, struct type_quant *quants,
                              struct span *span) {
    assert(of != NULL && span != NULL);
    struct type *res = type_new();
    res->kind = TYPE_POINTER;
    res->span = *span;
    res->pointer.of = of;
    res->pointer.quants = quants;
    return res;
}

struct type *type_func_new(struct type *ret_type,
                           struct type_func_param *params, struct span *span) {
    assert(ret_type != NULL && span != NULL);
    struct type *res = type_new();
    res->kind = TYPE_FUNC;
    res->span = *span;
    res->func.ret_type = ret_type;
    res->func.params = params;
    return res;
}

struct type_func_param *type_func_param_new() {
    struct type_func_param *param = malloc(sizeof(struct type_func_param));
    if (!param) fatal_error("memory allocation failed");
    return param;
}

struct type_func_param *type_func_param_normal_new(
    struct type *type, struct type_func_param_name *name, struct span *span) {
    assert(type != NULL && name != NULL && span != NULL);
    struct type_func_param *res = type_func_param_new();
    res->next = NULL;
    res->kind = TYPE_FUNC_PARAM_NORMAL;
    res->span = *span;
    res->type = type;
    res->name = *name;
    return res;
}

struct type_func_param *type_func_param_varidic_new(struct span *span) {
    assert(span != NULL);
    struct type_func_param *res = type_func_param_new();
    res->next = NULL;
    res->kind = TYPE_FUNC_PARAM_VARIDIC;
    res->span = *span;
    return res;
}

struct type *type_typedef_name_new(char *value, struct span *span) {
    assert(span != NULL);
    struct type *res = type_new();
    res->kind = TYPE_TYPEDEF_NAME;
    res->span = *span;
    res->typedef_name.value = value;
    return res;
}

struct type_quant *type_quant_new(enum type_quant_kind kind,
                                  struct span *span) {
    assert(span != NULL);
    struct type_quant *quant = malloc(sizeof(struct type_quant));
    if (!quant) fatal_error("memory allocation failed");
    quant->kind = kind;
    quant->span = *span;
    return quant;
}
