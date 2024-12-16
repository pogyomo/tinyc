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

#include <assert.h>
#include <tinyc/string.h>
#include <tinyc/token.h>

static inline bool is_single_token(struct tinyc_token *token) {
    return token->next == token && token->prev == token;
}

static void insert_token(void) {
    struct tinyc_span span;
    struct tinyc_token *token1 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token2 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token3 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token4 = tinyc_token_create_ident(&span, 0, "");

    assert(tinyc_token_insert(token1, token2) == token2);
    assert(tinyc_token_insert(token2, token3) == token3);
    assert(tinyc_token_insert(token3, token4) == token4);

    assert(token1->next == token2);
    assert(token2->next == token3);
    assert(token3->next == token4);
    assert(token4->next == token1);
    assert(token1->prev == token4);
    assert(token4->prev == token3);
    assert(token3->prev == token2);
    assert(token2->prev == token1);
}

static void insert_tokens(void) {
    struct tinyc_span span;
    struct tinyc_token *token1 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token2 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token3 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token4 = tinyc_token_create_ident(&span, 0, "");

    assert(tinyc_token_insert(token1, token2) == token2);
    assert(tinyc_token_insert(token2, token3) == token3);
    assert(tinyc_token_insert(token3, token4) == token4);

    assert(token1->next == token2);
    assert(token2->next == token3);
    assert(token3->next == token4);
    assert(token4->next == token1);
    assert(token1->prev == token4);
    assert(token4->prev == token3);
    assert(token3->prev == token2);
    assert(token2->prev == token1);
}

static void clone_range_one(void) {
    struct tinyc_span span;
    struct tinyc_token *token1 = tinyc_token_create_ident(&span, 0, "ident1");
    struct tinyc_token *cloned = tinyc_token_clone_range(token1, token1);
    assert(cloned != NULL);
    assert(tinyc_token_is_ident_of(cloned, "ident1"));
    assert(tinyc_token_is_ident_of(cloned->next, "ident1"));
    assert(tinyc_token_is_ident_of(cloned->prev, "ident1"));
}

static void clone_range_two(void) {
    struct tinyc_span span;
    struct tinyc_token *token1 = tinyc_token_create_ident(&span, 0, "ident1");
    struct tinyc_token *token2 = tinyc_token_create_ident(&span, 0, "ident2");
    tinyc_token_insert(token1, token2);
    struct tinyc_token *cloned = tinyc_token_clone_range(token1, token2);
    assert(cloned != NULL);
    assert(tinyc_token_is_ident_of(cloned, "ident1"));
    assert(tinyc_token_is_ident_of(cloned->next, "ident2"));
    assert(tinyc_token_is_ident_of(cloned->next->next, "ident1"));
    assert(tinyc_token_is_ident_of(cloned->prev, "ident2"));
    assert(tinyc_token_is_ident_of(cloned->prev->prev, "ident1"));
}

static void replace_token_with_token(void) {
    struct tinyc_span span;
    struct tinyc_token *token1 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token2 = tinyc_token_create_ident(&span, 0, "");

    assert(tinyc_token_replace(token1, token2) == token2);

    assert(is_single_token(token1));
    assert(token2->next == token2);
    assert(token2->prev == token2);
}

static void replace_token_with_tokens(void) {
    struct tinyc_span span;
    struct tinyc_token *token1 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token2 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token3 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token4 = tinyc_token_create_ident(&span, 0, "");

    tinyc_token_insert(token1, token2);
    tinyc_token_insert(token2, token3);
    assert(tinyc_token_replace(token4, token1) == token1);

    assert(is_single_token(token4));
    assert(token1->next == token2);
    assert(token2->next == token3);
    assert(token3->next == token1);
    assert(token1->prev == token3);
    assert(token3->prev == token2);
    assert(token2->prev == token1);
}

static void replace_tokens_with_token(void) {
    struct tinyc_span span;
    struct tinyc_token *token1 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token2 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token3 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token4 = tinyc_token_create_ident(&span, 0, "");

    tinyc_token_insert(token1, token4);
    tinyc_token_insert(token4, token3);
    assert(tinyc_token_replace(token4, token2) == token2);

    assert(is_single_token(token4));
    assert(token1->next == token2);
    assert(token2->next == token3);
    assert(token3->next == token1);
    assert(token1->prev == token3);
    assert(token3->prev == token2);
    assert(token2->prev == token1);
}

static void replace_tokens_with_tokens(void) {
    struct tinyc_span span;
    struct tinyc_token *token1 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token2 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token3 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token4 = tinyc_token_create_ident(&span, 0, "");
    struct tinyc_token *token5 = tinyc_token_create_ident(&span, 0, "");

    tinyc_token_insert(token1, token5);
    tinyc_token_insert(token5, token4);
    tinyc_token_insert(token2, token3);
    assert(tinyc_token_replace(token5, token2) == token2);

    assert(is_single_token(token5));
    assert(token1->next == token2);
    assert(token2->next == token3);
    assert(token3->next == token4);
    assert(token4->next == token1);
    assert(token1->prev == token4);
    assert(token4->prev == token3);
    assert(token3->prev == token2);
    assert(token2->prev == token1);
}

int main(void) {
    insert_token();
    insert_tokens();
    clone_range_one();
    clone_range_two();
    replace_token_with_token();
    replace_token_with_tokens();
    replace_tokens_with_token();
    replace_tokens_with_tokens();
}
