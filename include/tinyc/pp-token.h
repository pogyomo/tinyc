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

#ifndef TINYC_PP_TOKEN_H_
#define TINYC_PP_TOKEN_H_

#include "tinyc/span.h"
#include "tinyc/string.h"
#include "tinyc/token.h"

enum tinyc_pp_token_kind {
    TINYC_PP_TOKEN_NUMBER,
    TINYC_PP_TOKEN_HEADER,
    TINYC_PP_TOKEN_PUNCT,
    TINYC_PP_TOKEN_IDENT,
};

struct tinyc_pp_token {
    struct tinyc_pp_token *prev, *next;
    struct tinyc_span span;
    enum tinyc_pp_token_kind kind;
};

struct tinyc_pp_token_number {
    struct tinyc_pp_token token;
    struct tinyc_string value;
};

struct tinyc_pp_token_header {
    struct tinyc_pp_token token;
    bool is_std;               // True if <> style.
    struct tinyc_string path;  // Path inside <> or "".
};

struct tinyc_pp_token_punct {
    struct tinyc_pp_token token;
    enum tinyc_token_punct_kind kind;
};

struct tinyc_pp_token_ident {
    struct tinyc_pp_token token;
    struct tinyc_string value;
};

/// Create a pp-number token, returns pointer to header.
struct tinyc_pp_token *tinyc_pp_token_create_number(
    struct tinyc_pp_token *prev,
    struct tinyc_pp_token *next,
    struct tinyc_span span,
    struct tinyc_string value
);

/// Create a header token, returns pointer to header.
struct tinyc_pp_token *tinyc_pp_token_create_header(
    struct tinyc_pp_token *prev,
    struct tinyc_pp_token *next,
    struct tinyc_span span,
    bool is_std,
    struct tinyc_string path
);

/// Create a punctuation token, returns pointer to header.
struct tinyc_pp_token *tinyc_pp_token_create_punct(
    struct tinyc_pp_token *prev,
    struct tinyc_pp_token *next,
    struct tinyc_span span,
    enum tinyc_token_punct_kind kind
);

/// Create a identifier token, returns pointer to header.
struct tinyc_pp_token *tinyc_pp_token_create_ident(
    struct tinyc_pp_token *prev,
    struct tinyc_pp_token *next,
    struct tinyc_span span,
    struct tinyc_string value
);

#endif  // TINYC_PP_TOKEN_H_
