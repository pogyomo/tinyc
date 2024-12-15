// Copyright 2024 pogyomo
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TINYC_TOKEN_H_
#define TINYC_TOKEN_H_

#include "tinyc/span.h"
#include "tinyc/string.h"

enum tinyc_token_kind {
    TINYC_TOKEN_PUNCT,
    TINYC_TOKEN_IDENT,
    TINYC_TOKEN_KEYWORD,
    TINYC_TOKEN_STRING,
    TINYC_TOKEN_INT,
    TINYC_TOKEN_FLOAT,

    // Tokens used at preprocess
    TINYC_TOKEN_PP_NUMBER,
    TINYC_TOKEN_HEADER,
};

enum tinyc_token_punct_kind {
    TINYC_TOKEN_PUNCT_LSQUARE,    // "["
    TINYC_TOKEN_PUNCT_RSQUARE,    // "]"
    TINYC_TOKEN_PUNCT_LPAREN,     // "("
    TINYC_TOKEN_PUNCT_RPAREN,     // ")"
    TINYC_TOKEN_PUNCT_LCURLY,     // "{"
    TINYC_TOKEN_PUNCT_RCURLY,     // "}"
    TINYC_TOKEN_PUNCT_DOT,        // "."
    TINYC_TOKEN_PUNCT_ARROW,      // "->"
    TINYC_TOKEN_PUNCT_PPLUS,      // "++"
    TINYC_TOKEN_PUNCT_MMINUS,     // "--"
    TINYC_TOKEN_PUNCT_AMP,        // "&"
    TINYC_TOKEN_PUNCT_STAR,       // "*"
    TINYC_TOKEN_PUNCT_PLUS,       // "+"
    TINYC_TOKEN_PUNCT_MINUS,      // "-"
    TINYC_TOKEN_PUNCT_TILDE,      // "~"
    TINYC_TOKEN_PUNCT_EXC,        // "!"
    TINYC_TOKEN_PUNCT_SLASH,      // "/"
    TINYC_TOKEN_PUNCT_PERCENT,    // "%"
    TINYC_TOKEN_PUNCT_LSHIFT,     // "<<"
    TINYC_TOKEN_PUNCT_RSHIFT,     // ">>"
    TINYC_TOKEN_PUNCT_LT,         // "<"
    TINYC_TOKEN_PUNCT_GT,         // ">"
    TINYC_TOKEN_PUNCT_LE,         // "<="
    TINYC_TOKEN_PUNCT_GE,         // ">="
    TINYC_TOKEN_PUNCT_EQ,         // "=="
    TINYC_TOKEN_PUNCT_NE,         // "!="
    TINYC_TOKEN_PUNCT_HAT,        // "^"
    TINYC_TOKEN_PUNCT_VERT,       // "|"
    TINYC_TOKEN_PUNCT_AAMP,       // "&&"
    TINYC_TOKEN_PUNCT_VVERT,      // "||"
    TINYC_TOKEN_PUNCT_QUESTION,   // "?"
    TINYC_TOKEN_PUNCT_COLON,      // ":"
    TINYC_TOKEN_PUNCT_SEMICOLON,  // ";"
    TINYC_TOKEN_PUNCT_DDDOT,      // "..."
    TINYC_TOKEN_PUNCT_ASSIGN,     // "="
    TINYC_TOKEN_PUNCT_STAR_A,     // "*="
    TINYC_TOKEN_PUNCT_SLASH_A,    // "/="
    TINYC_TOKEN_PUNCT_PERCENT_A,  // "%="
    TINYC_TOKEN_PUNCT_PLUS_A,     // "+="
    TINYC_TOKEN_PUNCT_MINUS_A,    // "-="
    TINYC_TOKEN_PUNCT_LSHIFT_A,   // "<<="
    TINYC_TOKEN_PUNCT_RSHIFT_A,   // ">>="
    TINYC_TOKEN_PUNCT_AMP_A,      // "&="
    TINYC_TOKEN_PUNCT_HAT_A,      // "^="
    TINYC_TOKEN_PUNCT_VERT_A,     // "|="
    TINYC_TOKEN_PUNCT_COMMA,      // ","
    TINYC_TOKEN_PUNCT_SHARP,      // "#"
    TINYC_TOKEN_PUNCT_SSHARP,     // "##"
};

enum tinyc_token_keyword_kind {
    TINYC_TOKEN_KEYWORD_AUTO,        // "auto"
    TINYC_TOKEN_KEYWORD_BREAK,       // "break"
    TINYC_TOKEN_KEYWORD_CASE,        // "case"
    TINYC_TOKEN_KEYWORD_CHAR,        // "char"
    TINYC_TOKEN_KEYWORD_CONST,       // "const"
    TINYC_TOKEN_KEYWORD_CONTINUE,    // "continue"
    TINYC_TOKEN_KEYWORD_DEFAULT,     // "default"
    TINYC_TOKEN_KEYWORD_DO,          // "do"
    TINYC_TOKEN_KEYWORD_DOUBLE,      // "double"
    TINYC_TOKEN_KEYWORD_ELSE,        // "else"
    TINYC_TOKEN_KEYWORD_ENUM,        // "enum"
    TINYC_TOKEN_KEYWORD_EXTERN,      // "extern"
    TINYC_TOKEN_KEYWORD_FLOAT,       // "float"
    TINYC_TOKEN_KEYWORD_FOR,         // "for"
    TINYC_TOKEN_KEYWORD_GOTO,        // "goto"
    TINYC_TOKEN_KEYWORD_IF,          // "if"
    TINYC_TOKEN_KEYWORD_INLINE,      // "inline"
    TINYC_TOKEN_KEYWORD_INT,         // "int"
    TINYC_TOKEN_KEYWORD_LONG,        // "long"
    TINYC_TOKEN_KEYWORD_REGISTER,    // "register"
    TINYC_TOKEN_KEYWORD_RESTRICT,    // "restrict"
    TINYC_TOKEN_KEYWORD_RETURN,      // "return"
    TINYC_TOKEN_KEYWORD_SHORT,       // "short"
    TINYC_TOKEN_KEYWORD_SIGNED,      // "signed"
    TINYC_TOKEN_KEYWORD_SIZEOF,      // "sizeof"
    TINYC_TOKEN_KEYWORD_STATIC,      // "static"
    TINYC_TOKEN_KEYWORD_STRUCT,      // "struct"
    TINYC_TOKEN_KEYWORD_SWITCH,      // "switch"
    TINYC_TOKEN_KEYWORD_TYPEDEF,     // "typedef"
    TINYC_TOKEN_KEYWORD_UNION,       // "union"
    TINYC_TOKEN_KEYWORD_UNSIGNED,    // "unsigned"
    TINYC_TOKEN_KEYWORD_VOID,        // "void"
    TINYC_TOKEN_KEYWORD_VOLATILE,    // "volatile"
    TINYC_TOKEN_KEYWORD_WHILE,       // "while"
    TINYC_TOKEN_KEYWORD__BOOL,       // "_Bool"
    TINYC_TOKEN_KEYWORD__COMPLEX,    // "_Complex"
    TINYC_TOKEN_KEYWORD__IMAGINARY,  // "_Imaginary"
};

/// All token must contains this at beginning of its member, and these is
/// manipulated through pointer to this struct.
///
/// Token itself is a circularly-linked list, and each token can be visited via
/// next and prev.
struct tinyc_token {
    struct tinyc_token *prev, *next;
    struct tinyc_span span;
    enum tinyc_token_kind kind;
    int tspaces;  // Number of spaces after this token.
};

struct tinyc_token_punct {
    struct tinyc_token token;
    enum tinyc_token_punct_kind kind;
};

struct tinyc_token_ident {
    struct tinyc_token token;
    struct tinyc_string value;
};

struct tinyc_token_keyword {
    struct tinyc_token token;
    enum tinyc_token_keyword_kind kind;
};

struct tinyc_token_string {
    struct tinyc_token token;
    struct tinyc_string value;  // Unescaped string.
};

struct tinyc_token_int_value {
    // TODO: Add members
};

struct tinyc_token_int {
    struct tinyc_token token;
    struct tinyc_token_int_value value;
};

struct tinyc_token_float_value {
    // TODO: Add members
};

struct tinyc_token_float {
    struct tinyc_token token;
    struct tinyc_token_float_value value;
};

struct tinyc_token_pp_number {
    struct tinyc_token token;
    struct tinyc_string value;
};

struct tinyc_token_header {
    struct tinyc_token token;
    bool is_std;               // True if <> style.
    struct tinyc_string path;  // Path inside <> or "".
};

/// Insert tokens after it, returns first token in tokens.
struct tinyc_token *tinyc_token_insert(
    struct tinyc_token *it,
    struct tinyc_token *tokens
);

/// Replace it with tokens, returns first token in tokens.
/// Replaced token be a list contains only itself.
struct tinyc_token *tinyc_token_replace(
    struct tinyc_token *it,
    struct tinyc_token *tokens
);

/// Returns true if token is identifier.
bool tinyc_token_is_ident(struct tinyc_token *this);

/// Returns true if token is specified identifier.
bool tinyc_token_is_ident_of(struct tinyc_token *this, const char *expect);

/// Returns true if token is punctuation.
bool tinyc_token_is_punct(struct tinyc_token *this);

/// Returns true if token is punctuation of the kind.
bool tinyc_token_is_punct_of(
    struct tinyc_token *this,
    enum tinyc_token_punct_kind kind
);

/// Create a punctuation token, returns pointer to token.
struct tinyc_token *tinyc_token_create_punct(
    const struct tinyc_span *span,
    int tspaces,
    enum tinyc_token_punct_kind kind
);

/// Create a identifier token, returns pointer to token.
struct tinyc_token *tinyc_token_create_ident(
    const struct tinyc_span *span,
    int tspaces,
    const char *value
);

/// Create a keyword token, returns pointer to token.
struct tinyc_token *tinyc_token_create_keyword(
    const struct tinyc_span *span,
    int tspaces,
    enum tinyc_token_keyword_kind kind
);

/// Create a string token, returns pointer to token.
struct tinyc_token *tinyc_token_create_string(
    const struct tinyc_span *span,
    int tspaces,
    const char *value
);

/// Create a integer token, returns pointer to token.
struct tinyc_token *tinyc_token_create_int(
    const struct tinyc_span *span,
    int tspaces,
    const struct tinyc_token_int_value *value
);

/// Create a floating number token, returns pointer to token.
struct tinyc_token *tinyc_token_create_float(
    const struct tinyc_span *span,
    int tspaces,
    const struct tinyc_token_float_value *value
);

/// Create a pp-number token, returns pointer to token.
struct tinyc_token *tinyc_token_create_pp_number(
    const struct tinyc_span *span,
    int tspaces,
    const char *value
);

/// Create a header token, returns pointer to token.
struct tinyc_token *tinyc_token_create_header(
    const struct tinyc_span *span,
    int tspaces,
    bool is_std,
    const char *path
);

#endif  // TINYC_TOKEN_H_
