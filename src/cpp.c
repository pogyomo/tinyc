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

#include "tinyc/cpp.h"

#include <assert.h>
#include <string.h>

#include "tinyc/diag.h"
#include "tinyc/repo.h"
#include "tinyc/token.h"

static inline bool expect_token_next(
    const struct tinyc_repo *repo,
    struct tinyc_token *head,
    struct tinyc_token **it
) {
    if ((*it)->next == head) {
        tinyc_diag(
            TINYC_DIAG_ERROR,
            repo,
            &(*it)->span,
            "token not found",
            "expect token after this"
        );
        return false;
    } else {
        *it = (*it)->next;
        return true;
    }
}

static inline bool is_ident(struct tinyc_token *token) {
    return token->kind == TINYC_TOKEN_IDENT;
}

static inline bool expect_ident(
    const struct tinyc_repo *repo,
    struct tinyc_token *token
) {
    if (is_ident(token)) {
        return true;
    } else {
        tinyc_diag(
            TINYC_DIAG_ERROR,
            repo,
            &token->span,
            "unexpected token found",
            "expect identifier"
        );
        return false;
    }
}

static inline bool is_punct_of(
    struct tinyc_token *token,
    enum tinyc_token_punct_kind kind
) {
    return token->kind == TINYC_TOKEN_PUNCT &&
           ((struct tinyc_token_punct *)token)->kind == kind;
}

static inline struct tinyc_token *process_define(
    struct tinyc_cpp_context *ctx,
    const struct tinyc_repo *repo,
    struct tinyc_token *head,
    struct tinyc_token *it
) {
    bool spaces = it->tspaces > 0;
    if (!expect_token_next(repo, head, &it)) return NULL;
    if (is_punct_of(it, TINYC_TOKEN_PUNCT_LPAREN) && spaces) {
        // Parse function-like macro
    } else {
        // Parse normal macro
    }
    return NULL;
}

static inline struct tinyc_token *process_directive(
    struct tinyc_cpp_context *ctx,
    const struct tinyc_repo *repo,
    struct tinyc_token *head,
    struct tinyc_token *it
) {
    if (!expect_token_next(repo, head, &it)) return NULL;
    if (!expect_ident(repo, it)) return NULL;

    struct tinyc_token_ident *name = (struct tinyc_token_ident *)it;
    if (strcmp(name->value.cstr, "define")) {
        return process_define(ctx, repo, head, it);
    } else {
        tinyc_diag(TINYC_DIAG_ERROR, repo, &it->span, "unknown directive", "");
        return NULL;
    }
}

struct tinyc_token *tinyc_cpp(
    struct tinyc_cpp_context *ctx,
    const struct tinyc_repo *repo,
    struct tinyc_token *tokens
) {
    assert(tokens);
    if (is_punct_of(tokens, TINYC_TOKEN_PUNCT_SHARP)) {
        return process_directive(ctx, repo, tokens, tokens);
    } else {
        return tokens;
    }
}
