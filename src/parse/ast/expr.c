#include "expr.h"

#include <stdlib.h>

#include "../../panic.h"

struct expr *expr_new() {
    struct expr *expr = malloc(sizeof(struct expr));
    if (!expr) fatal_error("memory allocation failed");
    return expr;
}

struct expr *expr_infix_new(struct expr *lhs, struct expr *rhs,
                            struct expr_infix_op *op, struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_INFIX;
    res->span = *span;
    res->infix.lhs = lhs;
    res->infix.rhs = rhs;
    res->infix.op = *op;
    return res;
}

struct expr *expr_assign_new(struct expr *lhs, struct expr *rhs,
                             struct expr_assign_op *op, struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_ASSIGN;
    res->span = *span;
    res->assign.lhs = lhs;
    res->assign.rhs = rhs;
    res->assign.op = *op;
    return res;
}

struct expr *expr_cond_new(struct expr *cond, struct expr *then,
                           struct expr *else_, struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_COND;
    res->span = *span;
    res->cond.cond = cond;
    res->cond.then = then;
    res->cond.else_ = else_;
    return res;
}

struct expr *expr_unary_new(struct expr *expr, struct expr_unary_op *op,
                            struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_UNARY;
    res->span = *span;
    res->unary.expr = expr;
    res->unary.op = *op;
    return res;
}

struct expr *expr_sizeof_expr_new(struct expr *expr, struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_SIZEOF;
    res->span = *span;
    res->sizeof_.kind = EXPR_SIZEOF_EXPR;
    res->sizeof_.expr = expr;
    return res;
}

struct expr *expr_sizeof_type_new(struct type *type, struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_SIZEOF;
    res->span = *span;
    res->sizeof_.kind = EXPR_SIZEOF_TYPE;
    res->sizeof_.type = type;
    return res;
}

struct expr *expr_cast_new(struct type *type, struct expr *expr,
                           struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_CAST;
    res->span = *span;
    res->cast.type = type;
    res->cast.expr = expr;
    return res;
}

struct expr *expr_index_new(struct expr *body, struct expr *index,
                            struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_INDEX;
    res->span = *span;
    res->index.body = body;
    res->index.index = index;
    return res;
}

struct expr *expr_call_new(struct expr *body, struct expr_call_arg *args,
                           struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_CALL;
    res->span = *span;
    res->call.body = body;
    res->call.args = args;
    return res;
}

struct expr_call_arg *expr_call_arg_new(struct expr *value) {
    struct expr_call_arg *arg = malloc(sizeof(struct expr_call_arg));
    if (!arg) fatal_error("memory allocation failed");
    arg->next = NULL;
    arg->value = value;
    return arg;
}

struct expr *expr_access_new(struct expr *body, struct expr_access_op *op,
                             struct expr_access_field *field,
                             struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_ACCESS;
    res->span = *span;
    res->access.body = body;
    res->access.op = *op;
    res->access.field = *field;
    return res;
}

struct expr *expr_postfix_new(struct expr *expr, struct expr_postfix_op *op,
                              struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_POSTFIX;
    res->span = *span;
    res->postfix.expr = expr;
    res->postfix.op = *op;
    return res;
}

struct expr *expr_ident_new(struct string *value, struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_IDENT;
    res->span = *span;
    res->ident.value = *value;
    return res;
}

struct expr *expr_int_new(unsigned long long value, struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_INT;
    res->span = *span;
    res->int_.value = value;
    return res;
}

struct expr *expr_float_new(long double value, struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_FLOAT;
    res->span = *span;
    res->float_.value = value;
    return res;
}

struct expr *expr_char_new(struct string *value, struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_CHAR;
    res->span = *span;
    res->char_.value = *value;
    return res;
}

struct expr *expr_string_new(struct string *value, struct span *span) {
    struct expr *res = expr_new();
    res->kind = EXPR_STRING;
    res->span = *span;
    res->string.value = *value;
    return res;
}
