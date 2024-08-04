#ifndef PCC_PARSE_AST_H_
#define PCC_PARSE_AST_H_

#include <stdbool.h>

#include "../span.h"
#include "../string.h"

enum stmt_kind {
    STMT_EXPR,
    STMT_RETURN,
};

struct stmt {
    enum stmt_kind kind;
    struct span span;
};

enum expr_kind {
    // Postfix expressions
    EXPR_INDEX,
    EXPR_CALL,
    EXPR_ACCESS,
    EXPR_POSTFIX,

    // Primary expressions
    EXPR_IDENT,
    EXPR_INT,
    EXPR_FLOAT,
    EXPR_CHAR,
    EXPR_STRING,
};

struct expr {
    struct expr *next;
    enum expr_kind kind;
    struct span span;

    // Used when kind == EXPR_INDEX
    struct expr *index_body;
    struct expr *index_index;

    // Used when kind == EXPR_CALL
    struct expr *call_body;
    struct expr *call_args;

    // Used when kind == EXPR_ACCESS
    struct expr *access_body;
    struct expr *access_field;
    struct string access_field_val;

    // Used when kind == EXPR_IDENT
    struct string ident_val;

    // Used when kind == EXPR_INT
    unsigned long long int_val;

    // Used when kind == EXPR_FLOAT
    long double float_val;

    // Used when kind == EXPR_CHAR
    char char_val;

    // Used when kind == EXPR_STRING
    struct string string_val;
};

enum decl_kind {
    DECL_FUNC,
};

struct decl {
    enum decl_kind kind;
    struct span span;
};

struct unit {
    struct unit *next;
    struct decl decl;
};

#endif  // PCC_PARSE_AST_H_
