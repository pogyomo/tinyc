// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

// This file contains the definition of token, which is the minimal lexical
// element in c compiler, and will be used to parse the c program.

#ifndef TINYC_LEX_TOKEN_H_
#define TINYC_LEX_TOKEN_H_

#include <stdint.h>

#include "../span.h"
#include "../string.h"

enum token_kind {
    TK_PUNCT,    // +, -, ?, ...
    TK_KEYWORD,  // auto, struct, int, ...
    TK_INT,      // 10, -2, ...
    TK_FLOAT,    // 0.2, 1., 3e10, ...
    TK_IDENT,    // ident, Ident_20, ...
    TK_STRING,   // "hello world!", ...
    TK_CHAR,     // 'a', 'A', ...
};

enum token_punct_kind {
    TK_PUNCT_OR,            // ||
    TK_PUNCT_AND,           // &&
    TK_PUNCT_VERTICAL,      // |
    TK_PUNCT_HAT,           // ^
    TK_PUNCT_AMPERSAND,     // &
    TK_PUNCT_EQ,            // ==
    TK_PUNCT_NE,            // !=
    TK_PUNCT_LT,            // <
    TK_PUNCT_GT,            // >
    TK_PUNCT_LE,            // <=
    TK_PUNCT_GE,            // >=
    TK_PUNCT_LSHIFT,        // <<
    TK_PUNCT_RSHIFT,        // >>
    TK_PUNCT_PLUS,          // +
    TK_PUNCT_PLUSPLUS,      // ++
    TK_PUNCT_MINUS,         // -
    TK_PUNCT_MINUSMINUS,    // --
    TK_PUNCT_STAR,          // *
    TK_PUNCT_SLASH,         // /
    TK_PUNCT_PERCENT,       // %
    TK_PUNCT_LPAREN,        // (
    TK_PUNCT_RPAREN,        // )
    TK_PUNCT_LCURLY,        // {
    TK_PUNCT_RCURLY,        // }
    TK_PUNCT_LSQUARE,       // [
    TK_PUNCT_RSQUARE,       // ]
    TK_PUNCT_SEMICOLON,     // ;
    TK_PUNCT_COLON,         // :
    TK_PUNCT_COMMA,         // ,
    TK_PUNCT_TILDE,         // ~
    TK_PUNCT_NOT,           // !
    TK_PUNCT_QUESTION,      // ?
    TK_PUNCT_SHARP,         // #
    TK_PUNCT_DOT,           // .
    TK_PUNCT_ARROW,         // ->
    TK_PUNCT_ASSIGN,        // =
    TK_PUNCT_ORASSIGN,      // |=
    TK_PUNCT_XORASSIGN,     // ^=
    TK_PUNCT_ANDASSIGN,     // &=
    TK_PUNCT_LSHIFTASSIGN,  // <<=
    TK_PUNCT_RSHIFTASSIGN,  // >>=
    TK_PUNCT_ADDASSIGN,     // +=
    TK_PUNCT_SUBASSIGN,     // -=
    TK_PUNCT_MULASSIGN,     // *=
    TK_PUNCT_DIVASSIGN,     // /=
    TK_PUNCT_MODASSIGN,     // %=
};

enum token_keyword_kind {
    TK_KEYWORD_AUTO,        // auto
    TK_KEYWORD_BREAK,       // break
    TK_KEYWORD_CASE,        // case
    TK_KEYWORD_CHAR,        // char
    TK_KEYWORD_CONST,       // const
    TK_KEYWORD_CONTINUE,    // continue
    TK_KEYWORD_DEFAULT,     // default
    TK_KEYWORD_DO,          // do
    TK_KEYWORD_DOUBLE,      // double
    TK_KEYWORD_ELSE,        // else
    TK_KEYWORD_ENUM,        // enum
    TK_KEYWORD_EXTERN,      // extern
    TK_KEYWORD_FLOAT,       // float
    TK_KEYWORD_FOR,         // for
    TK_KEYWORD_GOTO,        // goto
    TK_KEYWORD_IF,          // if
    TK_KEYWORD_INLINE,      // inline
    TK_KEYWORD_INT,         // int
    TK_KEYWORD_LONG,        // long
    TK_KEYWORD_REGISTER,    // register
    TK_KEYWORD_RESTRICT,    // restrict
    TK_KEYWORD_RETURN,      // return
    TK_KEYWORD_SHORT,       // short
    TK_KEYWORD_SIGNED,      // signed
    TK_KEYWORD_SIZEOF,      // sizeof
    TK_KEYWORD_STATIC,      // static
    TK_KEYWORD_STRUCT,      // struct
    TK_KEYWORD_SWITCH,      // switch
    TK_KEYWORD_TYPEDEF,     // typedef
    TK_KEYWORD_UNION,       // union
    TK_KEYWORD_UNSIGNED,    // unsigned
    TK_KEYWORD_VOID,        // void
    TK_KEYWORD_VOLATILE,    // volatile
    TK_KEYWORD_WHILE,       // while
    TK_KEYWORD__BOOL,       // _Bool
    TK_KEYWORD__COMPLEX,    // _Complex
    TK_KEYWORD__IMAGINARY,  // _Imaginary
};

enum token_int_radix {
    TK_INT_OCT,
    TK_INT_DEC,
    TK_INT_HEX,
};

enum token_int_suffix {
    TK_INT_NONE,
    TK_INT_U,
    TK_INT_L,
    TK_INT_UL,
    TK_INT_LL,
    TK_INT_ULL,
};

enum token_float_radix {
    TK_FLOAT_DEC,
    TK_FLOAT_HEX,
};

enum token_float_suffix {
    TK_FLOAT_NONE,
    TK_FLOAT_F,
    TK_FLOAT_L,
};

struct token {
    struct token *next;
    enum token_kind kind;
    struct span span;

    // Used when kind == TK_IDENT
    struct token_ident {
        struct string value;
    } ident;

    // Used when kind == TK_CHAR
    struct token_char {
        struct string value;  // escaped ... in '...'.
    } char_;

    // Used when kind == TK_STRING
    struct token_string {
        struct string value;  // escaped ... in "...".
    } string;

    // Used when kind == TK_PUNCT
    struct token_punct {
        enum token_punct_kind kind;
    } punct;

    // Used when kind == TK_KEYWORD
    struct token_keyword {
        enum token_keyword_kind kind;
    } keyword;

    // Used when kind == TK_INT
    struct token_int {
        enum token_int_radix radix;
        enum token_int_suffix suffix;
        unsigned long long value;
    } int_;

    // Used when kind == TK_FLOAT
    struct token_float {
        enum token_float_radix radix;
        enum token_float_suffix suffix;
        long double value;
    } float_;
};

// Allocate memory for `token`, then return pointer to it.
// User should initialize kind specific variant after call this and before use
// it.
// This function never returns NULL, and instead of cause error.
struct token *token_new(enum token_kind kind, struct span *span);

// Iterate `head` and access each token with `iter`.
#define token_iter(iter, head) \
    for (struct token *iter = head; iter != NULL; iter = iter->next)

#endif  // TINYC_LEX_TOKEN_H_
