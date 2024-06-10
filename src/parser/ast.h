#ifndef TINYC_PARSER_AST_H_
#define TINYC_PARSER_AST_H_

#include <stdbool.h>

#include "../collections/string.h"
#include "../collections/vector.h"
#include "../lexer/token.h"
#include "../span.h"

// Forward declarations
typedef struct init init_t;
typedef struct expr expr_t;
typedef struct stmt stmt_t;
typedef struct decl decl_t;
typedef struct type type_t;

// ==================== initializer ====================

typedef struct {
    enum {
        DESIG_INDEX,   // .identifier
        DESIG_MEMBER,  // [constant-expression]
    } kind;
    union {
        struct {
            expr_t *expr;
        } index;
        struct {
            string_t name;
            span_t span;
        } member;
    };
} desig_t;

typedef struct {
    VECTOR(desig_t) desigs;
    init_t *init;
} init_list_item_t;

typedef struct init {
    enum {
        INIT_EXPR,  // assignment-expression
        INIT_LIST,  // { initializer-list }
    } kind;
    union {
        struct {
            expr_t *expr;
        } expr;
        struct {
            VECTOR(init_list_item_t) items;
        } list;
    };
    span_t span;
} init_t;

// ==================== expression ====================

typedef struct expr {
    enum {
        EXPR_UNARY,
        EXPR_INFIX,
        EXPR_POSTFIX,
        EXPR_ACCESS,
        EXPR_INDEX,
        EXPR_CALL,
        EXPR_COND,
        EXPR_SIZEOF,
        EXPR_CAST,
        EXPR_COMPOUND,
        EXPR_INTEGER,
        EXPR_FLOATING,
        EXPR_IDENT,
        EXPR_STRING,
        EXPR_CHARACTER,
    } kind;
    union {
        struct {
            struct {
                enum {
                    EXPR_UNARY_INC,    // ++
                    EXPR_UNARY_DEC,    // --
                    EXPR_UNARY_DEREF,  // *
                    EXPR_UNARY_REF,    // &
                    EXPR_UNARY_PLUS,   // +
                    EXPR_UNARY_MINUS,  // -
                    EXPR_UNARY_INV,    // ~
                    EXPR_UNARY_NEG,    // !
                } kind;
                span_t span;
            } op;
            expr_t *expr;
        } unary;
        struct {
            expr_t *lhs;
            struct {
                enum {
                    EXPR_INFIX_COMMA,         // ,
                    EXPR_INFIX_OR,            // ||
                    EXPR_INFIX_AND,           // &&
                    EXPR_INFIX_BITOR,         // |
                    EXPR_INFIX_BITXOR,        // ^
                    EXPR_INFIX_BITAND,        // &
                    EXPR_INFIX_EQ,            // ==
                    EXPR_INFIX_NE,            // !=
                    EXPR_INFIX_LT,            // <
                    EXPR_INFIX_GT,            // >
                    EXPR_INFIX_LE,            // <=
                    EXPR_INFIX_GE,            // >=
                    EXPR_INFIX_LSHIFT,        // <<
                    EXPR_INFIX_RSHIFT,        // >>
                    EXPR_INFIX_ADD,           // +
                    EXPR_INFIX_SUB,           // -
                    EXPR_INFIX_MUL,           // *
                    EXPR_INFIX_DIV,           // /
                    EXPR_INFIX_MOD,           // %
                    EXPR_INFIX_ASSIGN,        // =
                    EXPR_INFIX_ORASSIGN,      // |=
                    EXPR_INFIX_XORASSIGN,     // ^=
                    EXPR_INFIX_ANDASSIGN,     // &=
                    EXPR_INFIX_LSHIFTASSIGN,  // <<=
                    EXPR_INFIX_RSHIFTASSIGN,  // >>=
                    EXPR_INFIX_ADDASSIGN,     // +=
                    EXPR_INFIX_SUBASSIGN,     // -=
                    EXPR_INFIX_MULASSIGN,     // *=
                    EXPR_INFIX_DIVASSIGN,     // /=
                    EXPR_INFIX_MODASSIGN,     // %=
                } kind;
                span_t span;
            } op;
            expr_t *rhs;
        } infix;
        struct {
            expr_t *expr;
            struct {
                enum {
                    EXPR_POSTFIX_INC,
                    EXPR_POSTFIX_DEC,
                } kind;
                span_t span;
            } op;
        } postfix;
        struct {
            expr_t *expr;
            struct {
                enum {
                    EXPR_ACCESS_DOT,
                    EXPR_ACCESS_ARROW,
                } kind;
                span_t span;
            } op;
            struct {
                string_t name;
                span_t span;
            } field;
        } access;
        struct {
            expr_t *expr;
            expr_t *index;
        } index;
        struct {
            expr_t *expr;
            VECTOR(expr_t *) args;
        } call;
        struct {
            expr_t *cond;
            expr_t *then;
            expr_t *else_;
        } cond;
        struct {
            expr_t *expr;
        } sizeof_expr;
        struct {
            type_t *type;
        } sizeof_type;
        struct {
            type_t *type;
            expr_t *expr;
        } cast;
        struct {
            type_t *type;
            init_t *init;  // init->kind must be INIT_LIST.
        } compound;
        struct {
            unsigned long long value;
            int_suffix_t suffix;
            int_radix_t radix;
        } integer;
        struct {
            double value;
            float_suffix_t suffix;
            float_radix_t radix;
        } floating;
        struct {
            string_t value;
        } ident;
        struct {
            string_t value;  // Holds inner value of "...".
        } string;
        struct {
            string_t value;  // Holds inner value of '...'.
        } character;
    };
    span_t span;
} expr_t;

// Allocate proper size of memory for `expr_t`.
expr_t *expr_alloc();

// ==================== statement ====================

typedef struct {
    enum {
        STMT_BLOCK_ITEM_STMT,
        STMT_BLOCK_ITEM_DECL,
    } kind;
    union {
        stmt_t *stmt;
        decl_t *decl;
    };
} stmt_block_item_t;

typedef struct stmt {
    enum {
        STMT_LABELED,
        STMT_CASE,
        STMT_DEFAULT,
        STMT_EXPR,
        STMT_BLOCK,
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
    union {
        struct {
            struct {
                string_t name;
                span_t span;
            } label;
            stmt_t *stmt;
        } labeled;
        struct {
            expr_t *expr;
            stmt_t *stmt;
        } case_;
        struct {
            stmt_t *stmt;
        } default_;
        struct {
            expr_t *expr;  // NULL if this statement is empty statement.
        } expr;
        struct {
            VECTOR(stmt_block_item_t) items;
        } block;
        struct {
            expr_t *cond;
            stmt_t *then;
            stmt_t *else_;  // NULL if `else` doesn't appear in this if.
        } if_;
        struct {
            expr_t *expr;
            stmt_t *body;
        } switch_;
        struct {
            expr_t *cond;
            stmt_t *body;
        } while_;
        struct {
            expr_t *cond;
            stmt_t *body;
        } do_while;
        struct {
            struct {
                enum {
                    STMT_IF_INIT_EXPR,
                    STMT_IF_INIT_DECLS,
                } kind;
                union {
                    expr_t *expr;
                    VECTOR(decl_t *) decls;
                };
            } init;
            expr_t *cond;
            expr_t *update;
            stmt_t *body;
        } for_;
        struct {
            struct {
                string_t name;
                span_t span;
            } label;
        } goto_;
        struct {
            expr_t *expr;  // NULL if no return value specified.
        } return_;
    };
    span_t span;
} stmt_t;

// Allocate proper size of memory for `stmt_t`.
stmt_t *stmt_alloc();

// ==================== declaration ====================

typedef struct {
    enum {
        DECL_STORAGE_CLASS_TYPEDEF,
        DECL_STORAGE_CLASS_EXTERN,
        DECL_STORAGE_CLASS_STATIC,
        DECL_STORAGE_CLASS_AUTO,
        DECL_STORAGE_CLASS_REGISTER,
    } kind;
    span_t span;
} decl_storage_class_t;

typedef struct {
    type_t *type;
    decl_storage_class_t *storage_class;  // NULL if no storage class specified.
} decl_base_t;

typedef struct decl {
    enum {
        DECL_VAR,
        DECL_FUN,
    } kind;
    union {
        struct {
            struct {
                string_t *name;  // NULL if no name exist.
                span_t span;
            } name;
            init_t *init;  // NULL if no initializer exist.
        } var;
        struct {
            struct {
                string_t *name;  // NULL if no name exist.
                span_t span;
            } name;
            VECTOR(decl_t *) params;
            stmt_t *body;
        } fun;
    };
    decl_base_t base;
    span_t span;
} decl_t;

// Allocate proper size of memory for `decl_t`.
decl_t *decl_alloc();

// ==================== type ====================

typedef struct {
    string_t name;
    expr_t *value;  // NULL if no value assigned to this `name`.
    span_t span;
} enumerator_t;

typedef enum {
    TYPE_POINTER,
    TYPE_FUNCTION,
    TYPE_ARRAY,
    TYPE_STRUCT,
    TYPE_UNION,
    TYPE_ENUM,
    TYPE_VOID,
    TYPE_CHAR,
    TYPE_UCHAR,
    TYPE_SHORT,
    TYPE_USHORT,
    TYPE_INT,
    TYPE_UINT,
    TYPE_LONG,
    TYPE_ULONG,
    TYPE_LONGLONG,
    TYPE_ULONGLONG,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_LONGDOUBLE,
} type_kind_t;

typedef struct type {
    type_kind_t kind;
    bool is_const;
    bool is_restrict;
    bool is_volatile;
    union {
        struct {
            type_t *of;
        } pointer;
        struct {
            type_t *ret;
            VECTOR(decl_t *) params;
        } function;
        struct {
            type_t *of;
            expr_t *size;  // NULL if no size specified.
        } array;
        struct {
            string_t *name;               // NULL if no name specified.
            VECTOR_REF(decl_t *) fields;  // NULL if no fields provided.
        } struct_;
        struct {
            string_t *name;               // NULL if no name specified.
            VECTOR_REF(decl_t *) fields;  // NULL if no fields provided
        } union_;
        struct {
            string_t *name;                   // NULL if no name specified.
            VECTOR_REF(enumerator_t) fields;  // NULL if no field provided.
        } enum_;
    };
    span_t span;
} type_t;

// Allocate proper size of memory for `type_t`.
type_t *type_alloc();

#endif  // TINYC_PARSER_AST_H_
