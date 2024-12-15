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

#include "tinyc/cpp/helper.h"
#include "tinyc/cpp/macro.h"
#include "tinyc/diag.h"
#include "tinyc/string.h"
#include "tinyc/token.h"

bool tinyc_cpp_parse_define_args(
    struct tinyc_cpp_context *ctx,
    const struct tinyc_repo *repo,
    struct tinyc_token *head,
    struct tinyc_token **it,
    struct tinyc_cpp_macro_func_param **args
) {
    if (!tinyc_cpp_expect_token_next(repo, head, it)) return false;

    *args = NULL;
    for (size_t index = 0;; ++index) {
        if (tinyc_token_is_punct_of(*it, TINYC_TOKEN_PUNCT_RPAREN)) {
            break;
        } else if (!tinyc_cpp_expect_ident(repo, *it)) {
            return false;
        }

        struct tinyc_token_ident *name = (struct tinyc_token_ident *)(*it);
        struct tinyc_cpp_macro_func_param
            *arg = tinyc_cpp_macro_func_param_create(name->value.cstr, index);
        if (*args) arg->next = *args;
        *args = arg;

        if (!tinyc_cpp_expect_token_next(repo, head, it)) return false;
        if (tinyc_token_is_punct_of(*it, TINYC_TOKEN_PUNCT_RPAREN)) {
            break;
        } else if (tinyc_token_is_punct_of(*it, TINYC_TOKEN_PUNCT_COMMA)) {
            if (!tinyc_cpp_expect_token_next(repo, head, it)) return false;
        } else {
            tinyc_diag(
                TINYC_DIAG_ERROR,
                repo,
                &(*it)->span,
                "unknown token found",
                "expect this to be ) or ,"
            );
            return false;
        }
    }
    return true;
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

    if (tinyc_token_is_punct_of(*it, TINYC_TOKEN_PUNCT_LPAREN) && spaces) {
        struct tinyc_cpp_macro_func_param *args;
        if (!tinyc_cpp_parse_define_args(ctx, repo, head, it, &args)) {
            return false;
        }
        // TODO: Extract body
    } else {
        // TODO: Parse normal macro
    }
    return false;
}
