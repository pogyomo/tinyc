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

#include "tinyc/cpp/define.h"

#include <assert.h>

#include "tinyc/cpp/context.h"
#include "tinyc/cpp/helper.h"
#include "tinyc/cpp/macro.h"
#include "tinyc/cpp/params.h"
#include "tinyc/string.h"
#include "tinyc/token.h"

static inline bool parse_func(
    const char *name,
    struct tinyc_cpp_context *ctx,
    const struct tinyc_repo *repo,
    struct tinyc_token *head,
    struct tinyc_token **it
) {
    struct tinyc_cpp_macro_func_param *params;
    if (!tinyc_cpp_parse_params(ctx, repo, head, it, &params)) {
        return false;
    }
    return false;
}

static inline bool parse_normal(
    const char *name,
    struct tinyc_cpp_context *ctx,
    const struct tinyc_repo *repo,
    struct tinyc_token *head,
    struct tinyc_token **it
) {
    return false;
}

/// Parse define directive. it must point to directive name.
bool tinyc_cpp_parse_define(
    struct tinyc_cpp_context *ctx,
    const struct tinyc_repo *repo,
    struct tinyc_token *head,
    struct tinyc_token **it
) {
    bool spaces = (*it)->tspaces > 0;
    if (!tinyc_cpp_expect_token_next(repo, head, it)) return false;

    if (!tinyc_cpp_expect_ident(repo, *it)) return false;
    struct tinyc_token_ident *name = (struct tinyc_token_ident *)(*it);

    if ((*it)->next == head) {
        struct tinyc_cpp_macro *macro = tinyc_cpp_macro_normal_create(
            name->value.cstr,
            NULL
        );
        if (!macro) return false;
        tinyc_cpp_context_insert_macro(ctx, macro);
        return true;
    } else {
        *it = (*it)->next;
    }

    if (tinyc_token_is_punct_of(*it, TINYC_TOKEN_PUNCT_LPAREN) && spaces) {
        return parse_func(name->value.cstr, ctx, repo, head, it);
    } else {
        return parse_normal(name->value.cstr, ctx, repo, head, it);
    }
}
