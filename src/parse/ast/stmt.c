#include "stmt.h"

#include <stdlib.h>

#include "../../panic.h"

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
