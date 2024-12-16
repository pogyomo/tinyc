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

#include "tinyc/cpp/helper.h"

#include "tinyc/diag.h"
#include "tinyc/token.h"

bool tinyc_cpp_expect_token_next(
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

bool tinyc_cpp_expect_punct_of(
    const struct tinyc_repo *repo,
    struct tinyc_token *token,
    enum tinyc_token_punct_kind kind
) {
    if (tinyc_token_is_punct_of(token, kind)) {
        return true;
    } else {
        tinyc_diag(
            TINYC_DIAG_ERROR,
            repo,
            &token->span,
            "unexpected token found",
            "expect punctuation"
        );
        return false;
    }
}

bool tinyc_cpp_expect_ident(
    const struct tinyc_repo *repo,
    struct tinyc_token *token
) {
    if (tinyc_token_is_ident(token)) {
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
