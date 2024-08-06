// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_AST_FUN_DEF_H_
#define TINYC_PARSE_AST_FUN_DEF_H_

#include "../../span.h"
#include "type.h"

struct fun_def {
    struct span span;
    struct type *type;
    struct fun_def_name {
        char *value;
        struct span span;
    } name;
    struct fun_def_param {
        struct fun_def_param *next;
        enum fun_def_param_kind {
            FUN_DEF_PARAM_NORMAL,
            FUN_DEF_PARAM_VARIDIC,
        } kind;
        struct span span;

        // Used when kind == FUN_DEF_PARAM_NORMAL
        struct type *type;
        struct fun_def_param_name {
            bool exists;  // false if no name exists.
            char *value;
            struct span span;
        } name;
    } *params;          // NULL if no parameter exists.
    struct stmt *body;  // stmt->kind must be STMT_BLOCK.
};

struct fun_def *fun_def_new(struct type *type, struct fun_def_name *name,
                            struct fun_def_param *params, struct stmt *body,
                            struct span *span);
struct fun_def_param *fun_def_param_new();
struct fun_def_param *fun_def_param_normal_new(struct type *type,
                                               struct fun_def_param_name *name,
                                               struct span *span);
struct fun_def_param *fun_def_param_varidic_new(struct span *span);

#endif  // TINYC_PARSE_AST_FUN_DEF_H_
