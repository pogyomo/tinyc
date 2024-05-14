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
    TK_LPAREN,
    TK_RPAREN,
    TK_INTEGER,
    TK_IDENTIFIER,
    TK_STRING,
    TK_CHARACTER,
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
