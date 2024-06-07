#ifndef TINYC_LEXER_TOKEN_H_
#define TINYC_LEXER_TOKEN_H_

#include <stdbool.h>

#include "../collections/string.h"
#include "../span.h"

typedef enum {
    // Punctuations
    TK_OR,            // ||
    TK_AND,           // &&
    TK_VERTICAL,      // |
    TK_HAT,           // ^
    TK_AMPERSAND,     // &
    TK_EQ,            // ==
    TK_NE,            // !=
    TK_LT,            // <
    TK_GT,            // >
    TK_LE,            // <=
    TK_GE,            // >=
    TK_LSHIFT,        // <<
    TK_RSHIFT,        // >>
    TK_PLUS,          // +
    TK_PLUSPLUS,      // ++
    TK_MINUS,         // -
    TK_MINUSMINUS,    // --
    TK_STAR,          // *
    TK_SLASH,         // /
    TK_PERCENT,       // %
    TK_LPAREN,        // (
    TK_RPAREN,        // )
    TK_LCURLY,        // {
    TK_RCURLY,        // }
    TK_LSQUARE,       // [
    TK_RSQUARE,       // ]
    TK_SEMICOLON,     // ;
    TK_COLON,         // :
    TK_COMMA,         // ,
    TK_TILDE,         // ~
    TK_NOT,           // !
    TK_QUESTION,      // ?
    TK_SHARP,         // #
    TK_DOT,           // .
    TK_ARROW,         // ->
    TK_ASSIGN,        // =
    TK_ORASSIGN,      // |=
    TK_XORASSIGN,     // ^=
    TK_ANDASSIGN,     // &=
    TK_LSHIFTASSIGN,  // <<=
    TK_RSHIFTASSIGN,  // >>=
    TK_ADDASSIGN,     // +=
    TK_SUBASSIGN,     // -=
    TK_MULASSIGN,     // *=
    TK_DIVASSIGN,     // /=
    TK_MODASSIGN,     // %=

    // Keywords
    TK_AUTO,      // auto
    TK_BREAK,     // break
    TK_CASE,      // case
    TK_CHAR,      // char
    TK_CONST,     // const
    TK_CONTINUE,  // continue
    TK_DEFAULT,   // default
    TK_DO,        // do
    TK_DOUBLE,    // double
    TK_ELSE,      // else
    TK_ENUM,      // enum
    TK_EXTERN,    // extern
    TK_FLOAT,     // float
    TK_FOR,       // for
    TK_GOTO,      // goto
    TK_IF,        // if
    TK_INLINE,    // inline
    TK_INT,       // int
    TK_LONG,      // long
    TK_REGISTER,  // register
    TK_RESTRICT,  // restrict
    TK_RETURN,    // return
    TK_SHORT,     // short
    TK_SIGNED,    // signed
    TK_SIZEOF,    // sizeof
    TK_STATIC,    // static
    TK_STRUCT,    // struct
    TK_SWITCH,    // switch
    TK_TYPEDEF,   // typedef
    TK_UNION,     // union
    TK_UNSIGNED,  // unsigned
    TK_VOID,      // void
    TK_VOLATILE,  // volatile
    TK_WHILE,     // while

    // Literals
    TK_INTEGER,     // 10, -2, ...
    TK_FLOATING,    // 0.2, 1., 3e10, ...
    TK_IDENTIFIER,  // ident, Ident_20, ...
    TK_STRING,      // "hello world!", ...
    TK_CHARACTER,   // 'a', 'A', ...

    // Special
    TK_SPACE,    // whitespace
    TK_UNKNOWN,  // unknown character
} token_kind_t;

typedef enum {
    INT_RADIX_OCTAL,
    INT_RADIX_DECIMAL,
    INT_RADIX_HEXADECIMAL,
} int_radix_t;

typedef enum {
    INT_SUFFIX_NONE,
    INT_SUFFIX_U,
    INT_SUFFIX_L,
    INT_SUFFIX_UL,
    INT_SUFFIX_LL,
    INT_SUFFIX_ULL,
} int_suffix_t;

typedef enum {
    FLOAT_RADIX_DECIMAL,
    FLOAT_RADIX_HEXADECIMAL,
} float_radix_t;

typedef enum {
    FLOAT_SUFFIX_NONE,
    FLOAT_SUFFIX_F,
    FLOAT_SUFFIX_L,
} float_suffix_t;

typedef struct {
    token_kind_t kind;
    union {
        struct {
            int_radix_t radix;
            int_suffix_t suffix;
        } int_;  // Used when kind == TK_INTEGER
        struct {
            float_radix_t radix;
            float_suffix_t suffix;
        } float_;  // Used when kind == TK_FLOATING
    };
    string_t repr;       // Original string of this token.
    string_t printable;  // A printable type of `repr`.
    size_t lrow;
    span_t span;
} token_t;

// Returns string which head and tail `"` is removed from `value`.
string_t token_string_inner(token_t *token);

// Returns string which head and tail `'` is removed from `value`.
string_t token_character_inner(token_t *token);

#endif  // TINYC_LEXER_TOKEN_H_
