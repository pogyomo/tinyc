// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

// This file contains a definition of abstract syntax tree (AST) for c.
//
// The AST can represent invalid c program for conveniency, and it should be
// check in the future.
//
// The example of invalid c program which the AST can represent is as follow:
// - The struct or union declaration may contains function declaration.
// - The combination of type specifier maybe invalid.

#ifndef TINYC_PARSE_AST_H_
#define TINYC_PARSE_AST_H_

#include <stdbool.h>

#include "../span.h"
#include "../string.h"

// Used to initialize variables.
struct initializer {
    enum initializer_kind {
        INITIALIZER_EXPR,
        INITIALIZER_LIST,
    } kind;

    // Used when kind == INITIALIZER_EXPR
    struct expr *expr;

    // Used when kind == INITIALIZER_LIST
    struct initializer_list {
        struct initializer_list *next;

        struct designator {
            struct designator *next;
            enum designator_kind {
                DESIGNATOR_ARRAY,
                DESIGNATOR_IDENT,
            } kind;
            struct span span;

            // Used when kind == DESIGNATOR_ARRAY.
            // the form of `[ index ]`.
            unsigned long long index;

            // Used when kind == DESIGNATOR_IDENT.
            // the form of `. ident`.
            struct string ident;
        } *desigs;  // NULL if no designators exists.
        struct initializer *init;
    } *list;
};

struct type {
    enum type_kind {
        TYPE_BUILTIN,
        TYPE_STRUCT,
        TYPE_UNION,
        TYPE_ENUM,
        TYPE_POINTER,
        TYPE_ARRAY,
        TYPE_FUN_PTR,
    } kind;
    struct span span;
    struct type_quantifier {
        struct type_quantifier *next;
        enum type_quantifier_kind {
            TYPE_QUANTIFIER_CONST,
            TYPE_QUANTIFIER_RESTRICT,
            TYPE_QUANTIFIER_VOLATILE,
        } kind;
        struct span span;
    } *quantifiers;

    // Used when kind == TYPE_BUILTIN
    struct type_builtin {
        struct type_builtin_spec {
            struct type_builtin_spec *next;
            enum type_builtin_spec_kind {
                TYPE_BUILTIN_SPEC_VOID,
                TYPE_BUILTIN_SPEC_CHAR,
                TYPE_BUILTIN_SPEC_SHORT,
                TYPE_BUILTIN_SPEC_INT,
                TYPE_BUILTIN_SPEC_LONG,
                TYPE_BUILTIN_SPEC_FLOAT,
                TYPE_BUILTIN_SPEC_DOUBLE,
                TYPE_BUILTIN_SPEC_SIGNED,
                TYPE_BUILTIN_SPEC_UNSIGNED,
                TYPE_BUILTIN_SPEC__BOOL,
                TYPE_BUILTIN_SPEC__COMPLEX,
                TYPE_BUILTIN_SPEC_TYPEDEF_NAME,
            } kind;
            struct span span;

            // Used when kind == TYPE_BUILTIN_SPEC_TYPEDEF_NAME
            struct string typedef_name;
        } *specs;  // One or more specifiers. Validity is not checked.
    } builtin;

    // Used when kind == TYPE_STRUCT
    struct type_struct {
        struct type_struct_name {
            struct string value;
            struct span span;
        } *name;         // NULL if no name specified.
        bool is_opaque;  // true if { ... } omitted.
        struct type_struct_decl {
            struct type_struct_decl *next;
            struct decl *decl;  // kind maybe DECL_FUN, as c doesn't allow.
        } *decls;  // NULL if no declaration exists, or is_opaque is true.
    } struct_;

    // Used when kind == TYPE_UNION
    struct type_union {
        struct type_union_name {
            struct string value;
            struct span span;
        } *name;         // NULL if no name specified.
        bool is_opaque;  // true if { ... } omitted.
        struct type_union_decl {
            struct type_union_decl *next;
            struct decl *decl;  // kind maybe DECL_FUN, as c doesn't allow.
        } *decls;  // NULL if no declaration exists, or is_opaque is true.
    } union_;

    // Used when kind == TYPE_UNION
    struct type_enum {
        struct type_enum_name {
            struct string value;
            struct span span;
        } *name;         // NULL if no name specified.
        bool is_opaque;  // true if { ... } omitted.
        struct type_enum_enumerator {
            struct type_enum_enumerator *next;
        } *enumerators;  // NULL if no enumerator exists, or is_opaque is true.
    } enum_;

    // Used when kind == TYPE_POINTER
    struct type_pointer {
        struct type *of;
    } pointer;

    // Used when kind == TYPE_ARRAY
    struct type_array {
        struct type *of;
        struct type_array_size {
            enum type_array_size_kind {
                TYPE_ARRAY_SIZE_OMITTED,    // []
                TYPE_ARRAY_SIZE_SPECIFIED,  // [size]
                TYPE_ARRAY_SIZE_STAR,       // [*]
            } kind;
            struct span span;

            // Used when kind == TYPE_ARRAY_SIZE_SPECIFIED
            unsigned long long value;
        } size;
    } array;

    // Used when kind == TYPE_FUN_PTR
    struct type_fun_ptr {
        struct type *ret_type;
        struct type_fun_ptr_param {
            struct type_fun_ptr_param *next;
            struct type *type;
            bool is_variadic;  // if this is `...`. `next` will be NULL if true.
        } *params;
    } fun_ptr;
};

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

// Allocate memory for `stmt`.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_new();

// Create a new label statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_label_new(struct stmt_label_label *label, struct stmt *stmt,
                            struct span *span);

// Create a new case statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_case_new(struct expr *expr, struct stmt *stmt,
                           struct span *span);

// Create a new default statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_default_new(struct stmt *stmt, struct span *span);

// Create a new block statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_block_new(struct stmt_block_item *items, struct span *span);

// Create a new block item as declaration.
// Never returns NULL, and instead of cause error.
struct stmt_block_item *stmt_block_item_stmt_new(struct stmt *stmt);

// Create a new block item as declaration.
// Never returns NULL, and instead of cause error.
struct stmt_block_item *stmt_block_item_decl_new(struct decl *decl);

// Create a new expression statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_expr_new(struct expr *expr, struct span *span);

// Create a new if statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_if_new(struct expr *cond, struct stmt *then,
                         struct stmt *else_, struct span *span);

// Create a new switch statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_switch_new(struct expr *expr, struct stmt *body,
                             struct span *span);

// Create a new while statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_while_new(struct expr *cond, struct stmt *body,
                            struct span *span);

// Create a new do-while statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_do_while_new(struct expr *cond, struct stmt *body,
                               struct span *span);

// Create a new for statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_for_new(struct stmt_for_init *init, struct expr *cond,
                          struct expr *update, struct stmt *body,
                          struct span *span);

// Create a new for initializer as declaration.
// Never returns NULL, and instead of cause error.
struct stmt_for_init *stmt_for_init_decl_new(struct decl *decl);

// Create a new for initializer as expression.
// Never returns NULL, and instead of cause error.
struct stmt_for_init *stmt_for_init_expr_new(struct expr *expr);

// Create a new goto statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_goto_new(struct stmt_goto_label *label, struct span *span);

// Create a new continue statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_continue_new(struct span *span);

// Create a new break statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_break_new(struct span *span);

// Create a new return statement.
// Never returns NULL, and instead of cause error.
struct stmt *stmt_return_new(struct expr *expr, struct span *span);

struct expr {
    enum expr_kind {
        // Conditional expression
        EXPR_COND,

        // Infix expression
        EXPR_INFIX,
        EXPR_ASSIGN,

        // Unary expressions
        EXPR_UNARY,
        EXPR_SIZEOF_EXPR,
        EXPR_SIZEOF_TYPE,

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

    // Used when kind == EXPR_SIZEOF_TYPE
    struct type *sizeof_type;

    // Used when kind == EXPR_SIZEOF_EXPR
    struct expr *sizeof_expr;

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

// Allocate memory for `expr`.
// Never returns NULL, and instead of cause error.
struct expr *expr_new();

// Create a new conditional expression.
// Never returns NULL, and instead of cause error.
struct expr *expr_cond_new(struct expr *cond, struct expr *then,
                           struct expr *else_, struct span *span);

// Create a new infix expression.
// Never returns NULL, and instead of cause error.
struct expr *expr_infix_new(struct expr *lhs, struct expr *rhs,
                            struct expr_infix_op *op, struct span *span);

// Create a new assign expression.
// Never returns NULL, and instead of cause error.
struct expr *expr_assign_new(struct expr *lhs, struct expr *rhs,
                             struct expr_assign_op *op, struct span *span);

// Create a new unary expression.
// Never returns NULL, and instead of cause error.
struct expr *expr_unary_new(struct expr *expr, struct expr_unary_op *op,
                            struct span *span);

// Create a new index expression.
// Never returns NULL, and instead of cause error.
struct expr *expr_index_new(struct expr *body, struct expr *index,
                            struct span *span);

// Create a new call expression.
// Never returns NULL, and instead of cause error.
struct expr *expr_call_new(struct expr *body, struct expr_call_arg *args,
                           struct span *span);

// Create a new call argument.
// Never returns NULL, and instead of cause error.
struct expr_call_arg *expr_call_arg_new(struct expr *value);

// Create a new access expression.
// Never returns NULL, and instead of cause error.
struct expr *expr_access_new(struct expr *body, struct expr_access_op *op,
                             struct expr_access_field *field,
                             struct span *span);

// Create a new postfix expression.
// Never returns NULL, and instead of cause error.
struct expr *expr_postfix_new(struct expr *expr, struct expr_postfix_op *op,
                              struct span *span);

// Create a new identifier expression.
// Never returns NULL, and instead of cause error.
struct expr *expr_ident_new(struct string *value, struct span *span);

// Create a new integer expression.
// Never returns NULL, and instead of cause error.
struct expr *expr_int_new(unsigned long long value, struct span *span);

// Create a new floating number expression.
// Never returns NULL, and instead of cause error.
struct expr *expr_float_new(long double value, struct span *span);

// Create a new character expression.
// Never returns NULL, and instead of cause error.
struct expr *expr_char_new(struct string *value, struct span *span);

// Create a new string expression.
// Never returns NULL, and instead of cause error.
struct expr *expr_string_new(struct string *value, struct span *span);

// One or more declaration in a line.
struct decl {
    struct storage_class {
        enum storage_class_kind {
            STORAGE_CLASS_TYPEDEF,
            STORAGE_CLASS_EXTERN,
            STORAGE_CLASS_STATIC,
            STORAGE_CLASS_AUTO,
            STORAGE_CLASS_REGISTER,
        } kind;
        struct span span;
    } *storage_class;  // NULL if no storage class exists.
    struct function_spec {
        enum function_spec_kind {
            FUNCTION_SPEC_INLINE,
        } kind;
        struct span span;
    } *function_spec;  // NULL if no function specifier exists.

    // 0 or more declaration of function or variable.
    struct decl_item {
        struct decl_item *next;
        enum decl_kind {
            DECL_FUN,
            DECL_VAR,
        } kind;
        struct type *type;
        struct span span;

        // Used when kind == DECL_FUN
        struct decl_fun {
            struct decl_fun_name {
                struct string value;
                struct span span;
            } name;
            struct decl_fun_param {
                struct decl_fun_param *next;
                struct type *type;
                struct decl_fun_param_name {
                    struct string value;
                    struct span span;
                } name;
            } *params;
        } fun;

        // Used when kind == DECL_VAR
        struct decl_var {
            struct decl_var_name {
                struct string value;
                struct span span;
            } name;
            struct initializer *init;  // NULL if no initializer exists.
        } var;
    } *decls;
};

struct fun_def {
    struct span span;
    struct type *ret_type;
    struct fun_def_name {
        struct string value;
        struct span span;
    } name;
    struct fun_def_param {
        struct fun_def_param *next;
        struct type *type;
        struct fun_def_param_name {
            struct string value;
            struct span span;
        } *name;  // NULL if parameter name omitted.
    } *params;
    struct stmt *body;  // body->kind should be STMT_BLOCK
};

struct trans_unit {
    struct trans_unit *next;
    enum trans_unit_kind {
        TRANS_UNIT_DECL,
        TRANS_UNIT_FUN_DEF,
    } kind;

    // Used when kind == TRANS_UNIT_DECL
    struct decl *decl;

    // Used when kind == TRANS_UNIT_FUN_DEF
    struct fun_def *fun_def;
};

#endif  // TINYC_PARSE_AST_H_
