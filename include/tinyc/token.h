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

enum tinyc_token_header_kind {
    TINYC_TOKEN_PUNCT,
    TINYC_TOKEN_IDENT,
    TINYC_TOKEN_KEYWORD,
    TINYC_TOKEN_STRING,
};

enum tinyc_token_punct_kind {
    TINYC_TOKEN_LSQUARE,    // "["
    TINYC_TOKEN_RSQUARE,    // "]"
    TINYC_TOKEN_LPAREN,     // "("
    TINYC_TOKEN_RPAREN,     // ")"
    TINYC_TOKEN_LCURLY,     // "{"
    TINYC_TOKEN_RCURLY,     // "}"
    TINYC_TOKEN_DOT,        // "."
    TINYC_TOKEN_ARROW,      // "->"
    TINYC_TOKEN_PPLUS,      // "++"
    TINYC_TOKEN_MMINUS,     // "--"
    TINYC_TOKEN_AMP,        // "&"
    TINYC_TOKEN_STAR,       // "*"
    TINYC_TOKEN_PLUS,       // "+"
    TINYC_TOKEN_MINUS,      // "-"
    TINYC_TOKEN_TILDE,      // "~"
    TINYC_TOKEN_EXC,        // "!"
    TINYC_TOKEN_SLASH,      // "/"
    TINYC_TOKEN_PERCENT,    // "%"
    TINYC_TOKEN_LSHIFT,     // "<<"
    TINYC_TOKEN_RSHIFT,     // ">>"
    TINYC_TOKEN_LT,         // "<"
    TINYC_TOKEN_GT,         // ">"
    TINYC_TOKEN_LE,         // "<="
    TINYC_TOKEN_GE,         // ">="
    TINYC_TOKEN_EQ,         // "=="
    TINYC_TOKEN_NE,         // "!="
    TINYC_TOKEN_HAT,        // "^"
    TINYC_TOKEN_VERT,       // "|"
    TINYC_TOKEN_AAMP,       // "&&"
    TINYC_TOKEN_VVERT,      // "||"
    TINYC_TOKEN_QUESTION,   // "?"
    TINYC_TOKEN_COLON,      // ":"
    TINYC_TOKEN_SEMICOLON,  // ";"
    TINYC_TOKEN_DDDOT,      // "..."
    TINYC_TOKEN_ASSIGN,     // "="
    TINYC_TOKEN_STAR_A,     // "*="
    TINYC_TOKEN_SLASH_A,    // "/="
    TINYC_TOKEN_PERCENT_A,  // "%="
    TINYC_TOKEN_PLUS_A,     // "+="
    TINYC_TOKEN_MINUS_A,    // "-="
    TINYC_TOKEN_LSHIFT_A,   // "<<="
    TINYC_TOKEN_RSHIFT_A,   // ">>="
    TINYC_TOKEN_AMP_A,      // "&="
    TINYC_TOKEN_HAT_A,      // "^="
    TINYC_TOKEN_VERT_A,     // "|="
    TINYC_TOKEN_COMMA,      // ","
    TINYC_TOKEN_SHARP,      // "#"
    TINYC_TOKEN_SSHARP,     // "##"
};

enum tinyc_token_keyword_kind {
    TINYC_TOKEN_AUTO,        // "auto"
    TINYC_TOKEN_BREAK,       // "break"
    TINYC_TOKEN_CASE,        // "case"
    TINYC_TOKEN_CHAR,        // "char"
    TINYC_TOKEN_CONST,       // "const"
    TINYC_TOKEN_CONTINUE,    // "continue"
    TINYC_TOKEN_DEFAULT,     // "default"
    TINYC_TOKEN_DO,          // "do"
    TINYC_TOKEN_DOUBLE,      // "double"
    TINYC_TOKEN_ELSE,        // "else"
    TINYC_TOKEN_ENUM,        // "enum"
    TINYC_TOKEN_EXTERN,      // "extern"
    TINYC_TOKEN_FLOAT,       // "float"
    TINYC_TOKEN_FOR,         // "for"
    TINYC_TOKEN_GOTO,        // "goto"
    TINYC_TOKEN_IF,          // "if"
    TINYC_TOKEN_INLINE,      // "inline"
    TINYC_TOKEN_INT,         // "int"
    TINYC_TOKEN_LONG,        // "long"
    TINYC_TOKEN_REGISTER,    // "register"
    TINYC_TOKEN_RESTRICT,    // "restrict"
    TINYC_TOKEN_RETURN,      // "return"
    TINYC_TOKEN_SHORT,       // "short"
    TINYC_TOKEN_SIGNED,      // "signed"
    TINYC_TOKEN_SIZEOF,      // "sizeof"
    TINYC_TOKEN_STATIC,      // "static"
    TINYC_TOKEN_STRUCT,      // "struct"
    TINYC_TOKEN_SWITCH,      // "switch"
    TINYC_TOKEN_TYPEDEF,     // "typedef"
    TINYC_TOKEN_UNION,       // "union"
    TINYC_TOKEN_UNSIGNED,    // "unsigned"
    TINYC_TOKEN_VOID,        // "void"
    TINYC_TOKEN_VOLATILE,    // "volatile"
    TINYC_TOKEN_WHILE,       // "while"
    TINYC_TOKEN__BOOL,       // "_Bool"
    TINYC_TOKEN__COMPLEX,    // "_Complex"
    TINYC_TOKEN__IMAGINARY,  // "_Imaginary"
};

/// Struct all concrete header holds at beginning of its member.
/// All header is manipulated through pointer to this struct, and be downcasted
/// if concrete value required.
struct tinyc_token_header {
    struct tinyc_token_header *next, *prev;
    struct tinyc_span span;
    enum tinyc_token_header_kind kind;
};

struct tinyc_token_punct {
    struct tinyc_token_header header;
    enum tinyc_token_punct_kind kind;
};

struct tinyc_token_ident {
    struct tinyc_token_header header;
    struct tinyc_string value;
};

struct tinyc_token_keyword {
    struct tinyc_token_header header;
    enum tinyc_token_keyword_kind kind;
};

struct tinyc_token_string {
    struct tinyc_token_header header;
    struct tinyc_string value;
};

/// Create a punctuation token, returns pointer to header.
struct tinyc_token_header *tinyc_create_punct(
    struct tinyc_token_header *prev,
    struct tinyc_token_header *next,
    struct tinyc_span span,
    enum tinyc_token_punct_kind kind
);

/// Create a identifier token, returns pointer to header.
struct tinyc_token_header *tinyc_create_ident(
    struct tinyc_token_header *prev,
    struct tinyc_token_header *next,
    struct tinyc_span span,
    struct tinyc_string value
);

/// Create a keyword token, returns pointer to header.
struct tinyc_token_header *tinyc_create_keyword(
    struct tinyc_token_header *prev,
    struct tinyc_token_header *next,
    struct tinyc_span span,
    enum tinyc_token_keyword_kind kind
);

/// Create a string token, returns pointer to header.
struct tinyc_token_header *tinyc_create_string(
    struct tinyc_token_header *prev,
    struct tinyc_token_header *next,
    struct tinyc_span span,
    struct tinyc_string value
);

#endif  // TINYC_TOKEN_H_
