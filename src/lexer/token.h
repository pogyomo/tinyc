#ifndef TINYC_LEXER_TOKEN_H_
#define TINYC_LEXER_TOKEN_H_

#include "../collections/string.h"
#include "../span.h"

typedef enum {
    INT_SUFFIX_NONE,
    INT_SUFFIX_U,
    INT_SUFFIX_L,
    INT_SUFFIX_UL,
    INT_SUFFIX_LL,
    INT_SUFFIX_ULL,
} int_suffix_t;

typedef enum {
    INT_RADIX_OCTAL,
    INT_RADIX_DECIMAL,
    INT_RADIX_HEXADECIMAL,
} int_radix_t;

typedef struct {
    unsigned long long value;
    int_suffix_t suffix;
    int_radix_t radix;
} integer_item_t;

typedef struct {
    string_t *value;
} identifier_item_t;

typedef struct {
    string_t *value;
} string_item_t;

typedef struct {
    char value;
} character_item_t;

typedef struct {
} dummy_item_t;

typedef enum {
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
    TK_IDENTIFIER,  // ident, Ident_20, ...
    TK_STRING,      // "hello world!", ...
    TK_CHARACTER,   // 'a', 'A', ...

    // Special
    TK_SPACE,
} token_kind_t;

typedef union {
    integer_item_t integer;
    identifier_item_t identifier;
    string_item_t string;
    character_item_t character;
    dummy_item_t dummy;
} token_item_t;

typedef struct {
    int lrow;
    span_t span;
    token_kind_t kind;
    token_item_t item;
} token_t;

token_t *token_simple(token_kind_t kind, span_t span);
token_t *token_integer(integer_item_t item, span_t span);
token_t *token_identifier(identifier_item_t item, span_t span);
token_t *token_string(string_item_t item, span_t span);
token_t *token_character(character_item_t item, span_t span);

#endif  // TINYC_LEXER_TOKEN_H_
