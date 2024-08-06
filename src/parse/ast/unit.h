// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_AST_UNIT_H_
#define TINYC_PARSE_AST_UNIT_H_

#include "decl.h"
#include "fun_def.h"

struct trans_unit {
    struct trans_unit *next;
    enum trans_unit_kind {
        TRANS_UNIT_DECL,
        TRANS_UNIT_FUN_DEF,
    } kind;
    struct span span;

    // Used when kind == TRANS_UNIT_DECL
    struct decl *decl;  // NULL if nothingg declared.

    // Used when kind == TRANS_UNIT_FUN_DEF
    struct fun_def *fun_def;
};

struct trans_unit *trans_unit_new();
struct trans_unit *tran_unit_decl_new(struct decl *decl, struct span *span);
struct trans_unit *tran_unit_fun_def_new(struct fun_def *fun_def,
                                         struct span *span);

#endif  // TINYC_PARSE_AST_UNIT_H_
