// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_AST_STMT_H_
#define TINYC_PARSE_AST_STMT_H_

#include "../../span.h"
#include "../../string.h"

struct stmt {
    enum stmt_kind {
        STMT_LABEL,
        STMT_CASE,
        STMT_DEFAULT,
        STMT_BLOCK,
        STMT_EXPR,
        STMT_IF,
        STMT_SWITCH,
        STMT_WHILE,
        STMT_DO_WHILE,
        STMT_FOR,
        STMT_GOTO,
        STMT_CONTINUE,
        STMT_BREAK,
        STMT_RETURN,
    } kind;
    struct span span;

    // Used when kind == STMT_LABEL
    struct stmt_label {
        struct stmt_label_label {
            struct string value;
            struct span span;
        } label;
        struct stmt *stmt;
    } label;

    // Used when kind == STMT_CASE
    struct stmt_case {
        struct expr *expr;
        struct stmt *stmt;
    } case_;

    // Used when kind == STMT_DEFAULT
    struct stmt_default {
        struct stmt *stmt;
    } default_;

    // Used when kind == STMT_BLOCK
    struct stmt_block {
        struct stmt_block_item {
            struct stmt_block_item *next;
            enum stmt_block_item_kind {
                STMT_BLOCK_ITEM_STMT,
                STMT_BLOCK_ITEM_DECL,
            } kind;

            // Used when kind == STMT_BLOCK_ITEM_STMT
            struct stmt *stmt;

            // Used when kind == STMT_BLOCK_ITEM_DECL
            struct decl *decl;
        } *items;
    } block;

    // Used when kind == STMT_EXPR
    struct stmt_expr {
        struct expr *expr;  // NULL if it's empty statement.
    } expr;

    // Used when kind == EXPR_IF
    struct stmt_if {
        struct expr *cond;
        struct stmt *then;
        struct stmt *else_;  // NULL if `else` doesn't exists.
    } if_;

    // Used when kind == EXPR_SWITCH
    struct stmt_switch {
        struct expr *expr;
        struct stmt *body;
    } switch_;

    // Used when kind == STMT_WHILE
    struct stmt_while {
        struct expr *cond;
        struct stmt *body;
    } while_;

    // Used when kind == STMT_DO_WHILE
    struct stmt_do_while {
        struct expr *cond;
        struct stmt *body;
    } do_while;

    // Used when kind == STMT_FOR
    struct stmt_for {
        struct stmt_for_init {
            enum stmt_for_init_kind {
                STMT_FOR_INIT_EXPR,
                STMT_FOR_INIT_DECL,
            } kind;

            // Used when kind == STMT_FOR_INIT_EXPR
            struct expr *expr;

            // Used when kind == STMT_FOR_INIT_DECL
            struct decl *decl;
        } *init;              // NULL if it's omitted.
        struct expr *cond;    // NULL if it's omitted.
        struct expr *update;  // NULL if it's omitted.
        struct stmt *body;
    } for_;

    // Used when kind == STMT_GOTO
    struct stmt_goto {
        struct stmt_goto_label {
            struct string value;
            struct span span;
        } label;
    } goto_;

    // Used when kind == STMT_RETURN
    struct stmt_return {
        struct expr *expr;
    } return_;
};

struct stmt *stmt_new();
struct stmt *stmt_label_new(struct stmt_label_label *label, struct stmt *stmt,
                            struct span *span);
struct stmt *stmt_case_new(struct expr *expr, struct stmt *stmt,
                           struct span *span);
struct stmt *stmt_default_new(struct stmt *stmt, struct span *span);
struct stmt *stmt_block_new(struct stmt_block_item *items, struct span *span);
struct stmt_block_item *stmt_block_item_stmt_new(struct stmt *stmt);
struct stmt_block_item *stmt_block_item_decl_new(struct decl *decl);
struct stmt *stmt_expr_new(struct expr *expr, struct span *span);
struct stmt *stmt_if_new(struct expr *cond, struct stmt *then,
                         struct stmt *else_, struct span *span);
struct stmt *stmt_switch_new(struct expr *expr, struct stmt *body,
                             struct span *span);
struct stmt *stmt_while_new(struct expr *cond, struct stmt *body,
                            struct span *span);
struct stmt *stmt_do_while_new(struct expr *cond, struct stmt *body,
                               struct span *span);
struct stmt *stmt_for_new(struct stmt_for_init *init, struct expr *cond,
                          struct expr *update, struct stmt *body,
                          struct span *span);
struct stmt_for_init *stmt_for_init_decl_new(struct decl *decl);
struct stmt_for_init *stmt_for_init_expr_new(struct expr *expr);
struct stmt *stmt_goto_new(struct stmt_goto_label *label, struct span *span);
struct stmt *stmt_continue_new(struct span *span);
struct stmt *stmt_break_new(struct span *span);
struct stmt *stmt_return_new(struct expr *expr, struct span *span);

#endif  // TINYC_PARSE_AST_STMT_H_
