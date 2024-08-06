// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_AST_DECL_H_
#define TINYC_PARSE_AST_DECL_H_

#include <stdbool.h>

#include "../../span.h"

// initializer
struct initializer {
    enum initializer_kind {
        INITIALIZER_EXPR,
        INITIALIZER_LIST,
    } kind;
    struct span span;

    // Used when kind == INITIALIZER_EXPR
    struct expr *expr;

    // Used when kind == INITIALIZER_LIST
    struct initializer_list {
        struct initializer_list_item {
            struct initializer_list_item *next;
            struct span span;
            struct designator {
                struct designator *next;
                struct span span;
                enum designator_kind {
                    DESIGNATOR_INDEX,
                    DESIGNATOR_IDENT,
                } kind;

                // Used when kind == DESIGNATOR_INDEX
                struct expr *index;

                // Used when kind == DESIGNATOR_IDENT
                char *ident;
            } *desigs;  // NULL if no designator exists.
            struct initializer *value;
        } *items;  // NULL if no item exists
    } list;
};

struct initializer *initializer_new();
struct initializer *initializer_expr_new(struct expr *expr, struct span *span);
struct initializer *initializer_list_new(struct initializer_list *list,
                                         struct span *span);
struct initializer_list_item *initializer_list_item_new(
    struct designator *desigs, struct initializer *value, struct span *span);
struct designator *designator_new();
struct designator *designator_index_new(struct expr *index, struct span *span);
struct designator *designator_ident_new(char *ident, struct span *span);

// storage-class-specifier
struct storage_class {
    enum storage_class_kind {
        STORAGE_CLASS_TYPEDEF,
        STORAGE_CLASS_EXTERN,
        STORAGE_CLASS_STATIC,
        STORAGE_CLASS_AUTO,
        STORAGE_CLASS_REGISTER,
    } kind;
    struct span span;
};

struct storage_class *storage_class_new(enum storage_class_kind kind,
                                        struct span *span);

// function-specifier
struct function_spec {
    enum function_spec_kind {
        FUNCTION_SPEC_INLINE,
    } kind;
    struct span span;
};

struct function_spec *function_spec_new(enum function_spec_kind kind,
                                        struct span *span);

// declaration
struct decl {
    struct decl *next;
    enum decl_kind {
        DECL_VAR,
        DECL_FUN,
    } kind;
    struct span span;
    struct type *type;
    struct storage_class *class;  // NULL if no storage class specifier exists.

    // Used when kind == DECL_VAR
    struct decl_var {
        struct decl_var_name {
            char *value;
            struct span span;
        } name;
        struct initializer *init;  // NULL if no initializer exists.
    } var;

    // Used when kind == DECL_FUN
    struct decl_fun {
        struct function_spec *func_spec;  // NULL if no func specifier exists.
        struct decl_fun_name {
            char *value;
            struct span span;
        } name;
        struct decl_fun_param {
            struct decl_fun_param *next;
            enum decl_fun_param_kind {
                DECL_FUN_PARAM_NORMAL,
                DECL_FUN_PARAM_VARIDIC,
            } kind;
            struct span span;

            // Used when kind == DECL_FUN_PARAM_NORMAL
            struct type *type;
            struct decl_fun_param_name {
                bool exists;  // false if no name specified.
                char *value;
                struct span span;
            } name;
        } *params;  // NULL if no parameter exists
    } fun;
};

struct decl *decl_new();
struct decl *decl_var_new(struct type *type, struct storage_class *class,
                          struct decl_var_name *name, struct initializer *init,
                          struct span *span);
struct decl *decl_fun_new(struct type *type, struct storage_class *class,
                          struct function_spec *func_spec,
                          struct decl_fun_name *name,
                          struct decl_fun_param *params, struct span *span);
struct decl_fun_param *decl_fun_param_new();
struct decl_fun_param *decl_fun_param_normal_new(
    struct type *type, struct decl_fun_param_name *name, struct span *span);
struct decl_fun_param *decl_fun_param_varidic_new(struct span *span);

#endif  // TINYC_PARSE_AST_DECL_H_
