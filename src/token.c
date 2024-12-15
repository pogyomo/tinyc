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
#include <string.h>

#include "tinyc/string.h"

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

bool tinyc_token_is_ident(struct tinyc_token *this) {
    return this->kind == TINYC_TOKEN_IDENT;
}

bool tinyc_token_is_ident_of(struct tinyc_token *this, const char *expect) {
    return tinyc_token_is_ident(this) &&
           strcmp(((struct tinyc_token_ident *)this)->value.cstr, expect) == 0;
}

bool tinyc_token_is_punct(struct tinyc_token *this) {
    return this->kind == TINYC_TOKEN_PUNCT;
}

bool tinyc_token_is_punct_of(
    struct tinyc_token *this,
    enum tinyc_token_punct_kind kind
) {
    return tinyc_token_is_punct(this) &&
           ((struct tinyc_token_punct *)this)->kind == kind;
}

struct tinyc_token *tinyc_token_create_punct(
    const struct tinyc_span *span,
    int tspaces,
    enum tinyc_token_punct_kind kind
) {
    struct tinyc_token_punct *tk = malloc(sizeof(struct tinyc_token_punct));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = *span;
    tk->token.kind = TINYC_TOKEN_PUNCT;
    tk->token.tspaces = tspaces;
    tk->kind = kind;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_ident(
    const struct tinyc_span *span,
    int tspaces,
    const char *value
) {
    struct tinyc_token_ident *tk = malloc(sizeof(struct tinyc_token_ident));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = *span;
    tk->token.kind = TINYC_TOKEN_IDENT;
    tk->token.tspaces = tspaces;
    tinyc_string_from_copy(&tk->value, value);
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_keyword(
    const struct tinyc_span *span,
    int tspaces,
    enum tinyc_token_keyword_kind kind
) {
    struct tinyc_token_keyword *tk = malloc(sizeof(struct tinyc_token_keyword));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = *span;
    tk->token.kind = TINYC_TOKEN_KEYWORD;
    tk->token.tspaces = tspaces;
    tk->kind = kind;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_string(
    const struct tinyc_span *span,
    int tspaces,
    const char *value
) {
    struct tinyc_token_string *tk = malloc(sizeof(struct tinyc_token_string));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = *span;
    tk->token.kind = TINYC_TOKEN_STRING;
    tk->token.tspaces = tspaces;
    tinyc_string_from_copy(&tk->value, value);
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_int(
    const struct tinyc_span *span,
    int tspaces,
    const struct tinyc_token_int_value *value
) {
    struct tinyc_token_int *tk = malloc(sizeof(struct tinyc_token_int));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = *span;
    tk->token.kind = TINYC_TOKEN_INT;
    tk->token.tspaces = tspaces;
    tk->value = *value;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_float(
    const struct tinyc_span *span,
    int tspaces,
    const struct tinyc_token_float_value *value
) {
    struct tinyc_token_float *tk = malloc(sizeof(struct tinyc_token_float));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = *span;
    tk->token.kind = TINYC_TOKEN_FLOAT;
    tk->token.tspaces = tspaces;
    tk->value = *value;
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_pp_number(
    const struct tinyc_span *span,
    int tspaces,
    const char *value
) {
    struct tinyc_token_pp_number *tk = malloc(
        sizeof(struct tinyc_token_pp_number)
    );
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = *span;
    tk->token.kind = TINYC_TOKEN_PP_NUMBER;
    tk->token.tspaces = tspaces;
    tinyc_string_from_copy(&tk->value, value);
    return &tk->token;
}

struct tinyc_token *tinyc_token_create_header(
    const struct tinyc_span *span,
    int tspaces,
    bool is_std,
    const char *path
) {
    struct tinyc_token_header *tk = malloc(sizeof(struct tinyc_token_header));
    if (!tk) return NULL;
    tk->token.prev = tk->token.next = &tk->token;
    tk->token.span = *span;
    tk->token.kind = TINYC_TOKEN_HEADER;
    tk->token.tspaces = tspaces;
    tk->is_std = is_std;
    tinyc_string_from_copy(&tk->path, path);
    return &tk->token;
}
