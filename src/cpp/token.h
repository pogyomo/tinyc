// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_CPP_TOKEN_H_
#define TINYC_CPP_TOKEN_H_

#include <stdbool.h>

#include "../lex/token.h"
#include "../span.h"

enum cpp_token_kind {
    CPP_TK_IDENT,      // identifier
    CPP_TK_PP_NUMBER,  // pp-number
    CPP_TK_CHAR,       // unescaped character constant
    CPP_TK_STRING,     // unescaped string literal. Maybe contains backshash.
    CPP_TK_PUNCT,      // punctuator
    CPP_TK_SPACE,      // white-space or comment
    CPP_TK_NEWLINE,    // newline character
    CPP_TK_UNKNOWN,    // non-space character that is cannot be one of above
};

// Token for preprocessing.
//
// I don't holds header as a token because
// - `"..."` style of header is almost same as string literal except it can
// contains `\`, and I introduced `has_backslash` so it's possible to convert
// header and string literal freely.
// - `<...>` style of header can be constructed by concatenate tokens surrounded
// by `<` and `>`, as the preprocessing token completely maps the source code
// characters and it's possible to reconstruct source code from the tokens
// except backslash-newline is removed.
struct cpp_token {
    struct cpp_token *next;
    enum cpp_token_kind kind;
    struct span span;

    // Used when kind == CPP_TK_IDENT
    struct string ident_val;

    // Used when kind == CPP_TK_PP_NUMBER
    struct string pp_number_val;

    // Used when kind == CPP_TK_CHAR
    struct string char_val;  // unescaped ... in '...'.

    // Used when kind == CPP_TK_STRING
    struct string string_val;  // unescaped ... in "...".
    bool has_backslash;

    // Used when kind == CPP_TK_PUNCT
    enum token_punct_kind punct_kind;

    // Used when kind == CPP_TK_UNKNOWN
    char unknown_val;
};

// Allocate memory for `struct cpp_token` and return pointer to it.
// User should initialize its kind specific variant of the created token after
// call this and before use it.
// This function never returns NULL, and instead of cause error.
struct cpp_token *cpp_token_new(enum cpp_token_kind kind, struct span *span);

// Iterate `head` and access each token with `iter`.
#define cpp_token_iter(iter, head) \
    for (struct cpp_token *iter = head; iter != NULL; iter = iter->next)

#endif  // TINYC_CPP_TOKEN_H_
