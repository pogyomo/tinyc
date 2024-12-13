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

#define head(it) (it)
#define tail(it) ((it)->prev)

static void insert_between(
    struct tinyc_token *ld,
    struct tinyc_token *rd,
    struct tinyc_token *tokens
) {
    struct tinyc_token *ls = tokens, *rs = tokens->prev;
    ld->next = ls;
    rd->prev = rs;
    ls->prev = ld;
    rs->next = rd;
}

struct tinyc_token *tinyc_token_insert(
    struct tinyc_token *it,
    struct tinyc_token *tokens
) {
    insert_between(it, it->next, tokens);
    return tokens;
}

struct tinyc_token *tinyc_token_replace(
    struct tinyc_token *it,
    struct tinyc_token *tokens
) {
    if (it->next == it && it->prev == it) {
        return tokens;
    } else {
        insert_between(it->prev, it->next, tokens);
        it->next = it->prev = it;
        return tokens;
    }
}

struct tinyc_token *tinyc_token_create_punct(
    struct tinyc_span span,
    enum tinyc_token_punct_kind kind
) {
    struct tinyc_token_punct *tk = malloc(sizeof(struct tinyc_token_punct));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = span;
    tk->token.kind = TINYC_TOKEN_PUNCT;
    tk->kind = kind;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_ident(
    struct tinyc_span span,
    struct tinyc_string value
) {
    struct tinyc_token_ident *tk = malloc(sizeof(struct tinyc_token_ident));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = span;
    tk->token.kind = TINYC_TOKEN_IDENT;
    tk->value = value;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_keyword(
    struct tinyc_span span,
    enum tinyc_token_keyword_kind kind
) {
    struct tinyc_token_keyword *tk = malloc(sizeof(struct tinyc_token_keyword));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = span;
    tk->token.kind = TINYC_TOKEN_KEYWORD;
    tk->kind = kind;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_string(
    struct tinyc_span span,
    struct tinyc_string value
) {
    struct tinyc_token_string *tk = malloc(sizeof(struct tinyc_token_string));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = span;
    tk->token.kind = TINYC_TOKEN_STRING;
    tk->value = value;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_int(
    struct tinyc_span span,
    struct tinyc_token_int_value value
) {
    struct tinyc_token_int *tk = malloc(sizeof(struct tinyc_token_int));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = span;
    tk->token.kind = TINYC_TOKEN_INT;
    tk->value = value;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_float(
    struct tinyc_span span,
    struct tinyc_token_float_value value
) {
    struct tinyc_token_float *tk = malloc(sizeof(struct tinyc_token_float));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = span;
    tk->token.kind = TINYC_TOKEN_FLOAT;
    tk->value = value;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_pp_number(
    struct tinyc_span span,
    struct tinyc_string value
) {
    struct tinyc_token_pp_number *tk = malloc(
        sizeof(struct tinyc_token_pp_number)
    );
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = span;
    tk->token.kind = TINYC_TOKEN_PP_NUMBER;
    tk->value = value;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_header(
    struct tinyc_span span,
    bool is_std,
    struct tinyc_string path
) {
    struct tinyc_token_header *tk = malloc(sizeof(struct tinyc_token_header));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = span;
    tk->token.kind = TINYC_TOKEN_HEADER;
    tk->is_std = is_std;
    tk->path = path;
    return &tk->token;
}
