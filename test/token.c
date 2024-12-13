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

void insert_next(void) {
    struct tinyc_span span;
    struct tinyc_string s;
    struct tinyc_token *token1 = tinyc_token_create_ident(span, s);
    struct tinyc_token *token2 = tinyc_token_create_ident(span, s);
    struct tinyc_token *token3 = tinyc_token_create_ident(span, s);

    struct tinyc_token *it = token1;
    it = tinyc_token_insert_next(it, token2);
    assert(it == token2);
    it = tinyc_token_insert_next(it, token3);
    assert(it == token3);

    assert(token1->next == token2);
    assert(token2->next == token3);
    assert(token3->next == token1);
    assert(token1->prev == token3);
    assert(token3->prev == token2);
    assert(token2->prev == token1);
}

void insert_prev(void) {
    struct tinyc_span span;
    struct tinyc_string s;
    struct tinyc_token *token1 = tinyc_token_create_ident(span, s);
    struct tinyc_token *token2 = tinyc_token_create_ident(span, s);
    struct tinyc_token *token3 = tinyc_token_create_ident(span, s);

    struct tinyc_token *it = token1;
    it = tinyc_token_insert_prev(it, token3);
    assert(it == token3);
    it = tinyc_token_insert_prev(it, token2);
    assert(it == token2);

    assert(token1->next == token2);
    assert(token2->next == token3);
    assert(token3->next == token1);
    assert(token1->prev == token3);
    assert(token3->prev == token2);
    assert(token2->prev == token1);
}

void replace_single(void) {
    struct tinyc_span span;
    struct tinyc_string s;
    struct tinyc_token *token1 = tinyc_token_create_ident(span, s);
    struct tinyc_token *token2 = tinyc_token_create_ident(span, s);

    struct tinyc_token *it = token1;
    it = tinyc_token_replace(it, token2);
    assert(it == token2);

    assert(token1->next == token1);
    assert(token1->prev == token1);
    assert(token2->next == token2);
    assert(token2->prev == token2);
}

void replace_more(void) {
    struct tinyc_span span;
    struct tinyc_string s;
    struct tinyc_token *token1 = tinyc_token_create_ident(span, s);
    struct tinyc_token *token2 = tinyc_token_create_ident(span, s);
    struct tinyc_token *token3 = tinyc_token_create_ident(span, s);
    struct tinyc_token *token4 = tinyc_token_create_ident(span, s);

    struct tinyc_token *it = token1;
    it = tinyc_token_insert_next(it, token2);
    it = tinyc_token_insert_next(it, token3);
    it = tinyc_token_replace(it, token4);
    assert(it == token4);
    assert(token3->next == token3);
    assert(token3->prev == token3);

    assert(token1->next == token2);
    assert(token2->next == token4);
    assert(token4->next == token1);
    assert(token1->prev == token4);
    assert(token4->prev == token2);
    assert(token2->prev == token1);
}

int main(void) {
    insert_next();
    insert_prev();
    replace_single();
    replace_more();
}
