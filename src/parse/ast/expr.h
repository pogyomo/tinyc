// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_AST_EXPR_H_
#define TINYC_PARSE_AST_EXPR_H_

#include "../../span.h"
#include "../../string.h"

struct expr {
    enum expr_kind {
        // Conditional expression
        EXPR_COND,

        // Infix expression
        EXPR_INFIX,
        EXPR_ASSIGN,

        // Unary expressions
        EXPR_UNARY,
        EXPR_SIZEOF,

        // Cast expression
        EXPR_CAST,

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
    } kind;
    struct span span;

    // Used when kind == EXPR_COND
    struct expr_cond {
        struct expr *cond;
        struct expr *then;
        struct expr *else_;
    } cond;

    // Used when kind == EXPR_INFIX
    struct expr_infix {
        struct expr *lhs;
        struct expr *rhs;
        struct expr_infix_op {
            enum expr_infix_op_kind {
                EXPR_INFIX_OP_COMMA,   // `,`
                EXPR_INFIX_OP_OR,      // `||`
                EXPR_INFIX_OP_AND,     // `&&`
                EXPR_INFIX_OP_BITOR,   // `|`
                EXPR_INFIX_OP_BITXOR,  // `^`
                EXPR_INFIX_OP_BITAND,  // `&`
                EXPR_INFIX_OP_EQ,      // `==`
                EXPR_INFIX_OP_NE,      // `!=`
                EXPR_INFIX_OP_LT,      // `<`
                EXPR_INFIX_OP_GT,      // `>`
                EXPR_INFIX_OP_LE,      // `<=`
                EXPR_INFIX_OP_GE,      // `>=`
                EXPR_INFIX_OP_LSHIFT,  // `<<`
                EXPR_INFIX_OP_RSHIFT,  // `>>`
                EXPR_INFIX_OP_ADD,     // `+`
                EXPR_INFIX_OP_SUB,     // `-`
                EXPR_INFIX_OP_MUL,     // `*`
                EXPR_INFIX_OP_DIV,     // `/`
                EXPR_INFIX_OP_MOD,     // `%`
            } kind;
            struct span span;
        } op;
    } infix;

    // Used when kind == EXPR_ASSIGN
    struct expr_assign {
        struct expr *lhs;
        struct expr *rhs;
        struct expr_assign_op {
            enum expr_assign_op_kind {
                EXPR_ASSIGN_OP_NORMAL,  // `=`
                EXPR_ASSIGN_OP_ADD,     // `+=`
                EXPR_ASSIGN_OP_SUB,     // `-=`
                EXPR_ASSIGN_OP_MUL,     // `*=`
                EXPR_ASSIGN_OP_DIV,     // `/=`
                EXPR_ASSIGN_OP_MOD,     // `%=`
                EXPR_ASSIGN_OP_LSHIFT,  // `<<=`
                EXPR_ASSIGN_OP_RSHIFT,  // `>>=`
                EXPR_ASSIGN_OP_BITAND,  // `&=`
                EXPR_ASSIGN_OP_BITOR,   // `|=`
                EXPR_ASSIGN_OP_BITXOR,  // `^=`
            } kind;
            struct span span;
        } op;
    } assign;

    // Used when kind == EXPR_UNARY
    struct expr_unary {
        struct expr *expr;
        struct expr_unary_op {
            enum expr_unary_op_kind {
                EXPR_UNARY_OP_INC,    // `++`
                EXPR_UNARY_OP_DEC,    // `--`
                EXPR_UNARY_OP_REF,    // `&`
                EXPR_UNARY_OP_DEREF,  // `*`
                EXPR_UNARY_OP_PLUS,   // `+`
                EXPR_UNARY_OP_MINUS,  // `-`
                EXPR_UNARY_OP_INV,    // `~`
                EXPR_UNARY_OP_NEG,    // `!`
            } kind;
            struct span span;
        } op;
    } unary;

    // Used when kind == EXPR_SIZEOF
    struct expr_sizeof {
        enum expr_sizeof_kind {
            EXPR_SIZEOF_EXPR,
            EXPR_SIZEOF_TYPE,
        } kind;

        // Used when kind == EXPR_SIZEOF_EXPR
        struct expr *expr;

        // Used when kind == EXPR_SIZEOF_TYPE
        struct type *type;
    } sizeof_;

    // Used when kind == EXPR_CAST
    struct expr_cast {
        struct type *type;
        struct expr *expr;
    } cast;

    // Used when kind == EXPR_INDEX
    struct expr_index {
        struct expr *body;
        struct expr *index;
    } index;

    // Used when kind == EXPR_CALL
    struct expr_call {
        struct expr *body;
        struct expr_call_arg {
            struct expr_call_arg *next;
            struct expr *value;
        } *args;
    } call;

    // Used when kind == EXPR_ACCESS
    struct expr_access {
        struct expr *body;
        struct expr_access_op {
            enum expr_access_op_kind {
                EXPR_ACCESS_OP_DOT,
                EXPR_ACCESS_OP_ARROW,
            } kind;
            struct span span;
        } op;
        struct expr_access_field {
            struct string value;
            struct span span;
        } field;
    } access;

    // Used when kind == EXPR_POSTFIX
    struct expr_postfix {
        struct expr *expr;
        struct expr_postfix_op {
            enum expr_postfix_op_kind {
                EXPR_POSTFIX_OP_INC,  // `++`
                EXPR_POSTFIX_OP_DEC,  // `--`
            } kind;
            struct span span;
        } op;
    } postfix;

    // Used when kind == EXPR_IDENT
    struct expr_ident {
        struct string value;
    } ident;

    // Used when kind == EXPR_INT
    struct expr_int {
        unsigned long long value;
    } int_;

    // Used when kind == EXPR_FLOAT
    struct expr_float {
        long double value;
    } float_;

    // Used when kind == EXPR_CHAR
    struct expr_char {
        struct string value;
    } char_;

    // Used when kind == EXPR_STRING
    struct expr_string {
        struct string value;
    } string;
};

struct expr *expr_new();
struct expr *expr_cond_new(struct expr *cond, struct expr *then,
                           struct expr *else_, struct span *span);
struct expr *expr_infix_new(struct expr *lhs, struct expr *rhs,
                            struct expr_infix_op *op, struct span *span);
struct expr *expr_assign_new(struct expr *lhs, struct expr *rhs,
                             struct expr_assign_op *op, struct span *span);
struct expr *expr_unary_new(struct expr *expr, struct expr_unary_op *op,
                            struct span *span);
struct expr *expr_sizeof_expr_new(struct expr *expr, struct span *span);
struct expr *expr_sizeof_type_new(struct type *type, struct span *span);
struct expr *expr_cast_new(struct type *type, struct expr *expr,
                           struct span *span);
struct expr *expr_index_new(struct expr *body, struct expr *index,
                            struct span *span);
struct expr *expr_call_new(struct expr *body, struct expr_call_arg *args,
                           struct span *span);
struct expr_call_arg *expr_call_arg_new(struct expr *value);
struct expr *expr_access_new(struct expr *body, struct expr_access_op *op,
                             struct expr_access_field *field,
                             struct span *span);
struct expr *expr_postfix_new(struct expr *expr, struct expr_postfix_op *op,
                              struct span *span);
struct expr *expr_ident_new(struct string *value, struct span *span);
struct expr *expr_int_new(unsigned long long value, struct span *span);
struct expr *expr_float_new(long double value, struct span *span);
struct expr *expr_char_new(struct string *value, struct span *span);
struct expr *expr_string_new(struct string *value, struct span *span);

#endif  // TINYC_PARSE_AST_EXPR_H_
