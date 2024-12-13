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

#include "tinyc/token.h"

#include <stdlib.h>

struct tinyc_token *tinyc_token_create_punct(
    struct tinyc_token *prev,
    struct tinyc_token *next,
    struct tinyc_span span,
    enum tinyc_token_punct_kind kind
) {
    struct tinyc_token_punct *tk = malloc(sizeof(struct tinyc_token_punct));
    if (!tk) return NULL;
    tk->token.prev = prev;
    tk->token.next = next;
    tk->token.span = span;
    tk->token.kind = TINYC_TOKEN_PUNCT;
    tk->kind = kind;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_ident(
    struct tinyc_token *prev,
    struct tinyc_token *next,
    struct tinyc_span span,
    struct tinyc_string value
) {
    struct tinyc_token_ident *tk = malloc(sizeof(struct tinyc_token_ident));
    if (!tk) return NULL;
    tk->token.prev = prev;
    tk->token.next = next;
    tk->token.span = span;
    tk->token.kind = TINYC_TOKEN_IDENT;
    tk->value = value;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_keyword(
    struct tinyc_token *prev,
    struct tinyc_token *next,
    struct tinyc_span span,
    enum tinyc_token_keyword_kind kind
) {
    struct tinyc_token_keyword *tk = malloc(sizeof(struct tinyc_token_keyword));
    if (!tk) return NULL;
    tk->token.prev = prev;
    tk->token.next = next;
    tk->token.span = span;
    tk->token.kind = TINYC_TOKEN_KEYWORD;
    tk->kind = kind;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_string(
    struct tinyc_token *prev,
    struct tinyc_token *next,
    struct tinyc_span span,
    struct tinyc_string value
) {
    struct tinyc_token_string *tk = malloc(sizeof(struct tinyc_token_string));
    if (!tk) return NULL;
    tk->token.prev = prev;
    tk->token.next = next;
    tk->token.span = span;
    tk->token.kind = TINYC_TOKEN_STRING;
    tk->value = value;
    return &tk->token;
}
