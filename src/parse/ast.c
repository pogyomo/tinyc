#include "ast.h"

#include <stdlib.h>

#include "../panic.h"

struct stmt *stmt_new() {
    struct stmt *stmt = malloc(sizeof(struct stmt));
    if (!stmt) fatal_error("memory allocation failed");
    return stmt;
}

struct stmt *stmt_label_new(struct stmt_label_label *label, struct stmt *stmt,
                            struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_LABEL;
    res->span = *span;
    res->label.stmt = stmt;
    res->label.label = *label;
    return res;
}

struct stmt *stmt_case_new(struct expr *expr, struct stmt *stmt,
                           struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_CASE;
    res->span = *span;
    res->case_.expr = expr;
    res->case_.stmt = stmt;
    return res;
}

struct stmt *stmt_default_new(struct stmt *stmt, struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_DEFAULT;
    res->span = *span;
    res->default_.stmt = stmt;
    return res;
}

struct stmt *stmt_block_new(struct stmt_block_item *items, struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_BLOCK;
    res->span = *span;
    res->block.items = items;
    return res;
}

struct stmt_block_item *stmt_block_item_stmt_new(struct stmt *stmt) {
    struct stmt_block_item *item = malloc(sizeof(struct stmt_block_item));
    if (!item) fatal_error("memory allocation failed");
    item->next = NULL;
    item->kind = STMT_BLOCK_ITEM_STMT;
    item->stmt = stmt;
    return item;
}

struct stmt_block_item *stmt_block_item_decl_new(struct decl *decl) {
    struct stmt_block_item *item = malloc(sizeof(struct stmt_block_item));
    if (!item) fatal_error("memory allocation failed");
    item->next = NULL;
    item->kind = STMT_BLOCK_ITEM_DECL;
    item->decl = decl;
    return item;
}

struct stmt *stmt_expr_new(struct expr *expr, struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_EXPR;
    res->span = *span;
    res->expr.expr = expr;
    return res;
}

struct stmt *stmt_if_new(struct expr *cond, struct stmt *then,
                         struct stmt *else_, struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_IF;
    res->span = *span;
    res->if_.cond = cond;
    res->if_.then = then;
    res->if_.else_ = else_;
    return res;
}

struct stmt *stmt_switch_new(struct expr *expr, struct stmt *body,
                             struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_SWITCH;
    res->span = *span;
    res->switch_.expr = expr;
    res->switch_.body = body;
    return res;
}

struct stmt *stmt_while_new(struct expr *cond, struct stmt *body,
                            struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_WHILE;
    res->span = *span;
    res->while_.cond = cond;
    res->while_.body = body;
    return res;
}

struct stmt *stmt_do_while_new(struct expr *cond, struct stmt *body,
                               struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_DO_WHILE;
    res->span = *span;
    res->do_while.cond = cond;
    res->do_while.body = body;
    return res;
}

struct stmt *stmt_for_new(struct stmt_for_init *init, struct expr *cond,
                          struct expr *update, struct stmt *body,
                          struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_FOR;
    res->span = *span;
    res->for_.init = init;
    res->for_.cond = cond;
    res->for_.update = update;
    res->for_.body = body;
    return res;
}

struct stmt *stmt_goto_new(struct stmt_goto_label *label, struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_GOTO;
    res->span = *span;
    res->goto_.label = *label;
    return res;
}

struct stmt_for_init *stmt_for_init_decl_new(struct decl *decl) {
    struct stmt_for_init *init = malloc(sizeof(struct stmt_for_init));
    if (!init) fatal_error("memory allocation failed");
    init->kind = STMT_FOR_INIT_DECL;
    init->decl = decl;
    return init;
}

struct stmt_for_init *stmt_for_init_expr_new(struct expr *expr) {
    struct stmt_for_init *init = malloc(sizeof(struct stmt_for_init));
    if (!init) fatal_error("memory allocation failed");
    init->kind = STMT_FOR_INIT_EXPR;
    init->expr = expr;
    return init;
}

struct stmt *stmt_continue_new(struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_CONTINUE;
    res->span = *span;
    return res;
}

struct stmt *stmt_break_new(struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_BREAK;
    res->span = *span;
    return res;
}

struct stmt *stmt_return_new(struct expr *expr, struct span *span) {
    struct stmt *res = stmt_new();
    res->kind = STMT_RETURN;
    res->span = *span;
    res->return_.expr = expr;
    return res;
}

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
