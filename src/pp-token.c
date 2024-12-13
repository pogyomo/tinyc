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

#include "tinyc/pp-token.h"

#include <stdlib.h>

struct tinyc_pp_token *tinyc_pp_token_create_number(
    struct tinyc_pp_token *prev,
    struct tinyc_pp_token *next,
    struct tinyc_span span,
    struct tinyc_string value
) {
    struct tinyc_pp_token_number *token = malloc(
        sizeof(struct tinyc_pp_token_number)
    );
    if (!token) return NULL;
    token->token.prev = prev;
    token->token.next = next;
    token->token.span = span;
    token->token.kind = TINYC_PP_TOKEN_NUMBER;
    token->value = value;
    return &token->token;
}

struct tinyc_pp_token *tinyc_pp_token_create_header(
    struct tinyc_pp_token *prev,
    struct tinyc_pp_token *next,
    struct tinyc_span span,
    bool is_std,
    struct tinyc_string path
) {
    struct tinyc_pp_token_header *token = malloc(
        sizeof(struct tinyc_pp_token_header)
    );
    if (!token) return NULL;
    token->token.prev = prev;
    token->token.next = next;
    token->token.span = span;
    token->token.kind = TINYC_PP_TOKEN_HEADER;
    token->is_std = is_std;
    token->path = path;
    return &token->token;
}

struct tinyc_pp_token *tinyc_pp_token_create_punct(
    struct tinyc_pp_token *prev,
    struct tinyc_pp_token *next,
    struct tinyc_span span,
    enum tinyc_token_punct_kind kind
) {
    struct tinyc_pp_token_punct *token = malloc(
        sizeof(struct tinyc_pp_token_punct)
    );
    if (!token) return NULL;
    token->token.prev = prev;
    token->token.next = next;
    token->token.span = span;
    token->token.kind = TINYC_PP_TOKEN_PUNCT;
    token->kind = kind;
    return &token->token;
}

struct tinyc_pp_token *tinyc_pp_token_create_ident(
    struct tinyc_pp_token *prev,
    struct tinyc_pp_token *next,
    struct tinyc_span span,
    struct tinyc_string value
) {
    struct tinyc_pp_token_ident *token = malloc(
        sizeof(struct tinyc_pp_token_ident)
    );
    if (!token) return NULL;
    token->token.prev = prev;
    token->token.next = next;
    token->token.span = span;
    token->token.kind = TINYC_PP_TOKEN_IDENT;
    token->value = value;
    return &token->token;
}
