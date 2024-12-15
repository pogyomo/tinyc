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

#include "tinyc/cpp/define.h"
#include "tinyc/cpp/helper.h"
#include "tinyc/diag.h"
#include "tinyc/repo.h"
#include "tinyc/token.h"

static struct {
    const char *name;
    bool (*parser)(
        struct tinyc_cpp_context *ctx,
        const struct tinyc_repo *repo,
        struct tinyc_token *head,
        struct tinyc_token **it
    );
} parsers[] = {
    {"define", tinyc_cpp_parse_define},
};

static inline bool parse_directive(
    struct tinyc_cpp_context *ctx,
    const struct tinyc_repo *repo,
    struct tinyc_token *head,
    struct tinyc_token **it
) {
    if (!tinyc_cpp_expect_token_next(repo, head, it)) return false;

    for (size_t i = 0; i < sizeof(parsers) / sizeof(parsers[0]); ++i) {
        if (tinyc_token_is_ident_of(*it, parsers[i].name)) {
            return parsers[i].parser(ctx, repo, head, it);
        }
    }
    tinyc_diag(TINYC_DIAG_ERROR, repo, &(*it)->span, "unknown directive", "");
    return false;
}

bool tinyc_cpp(
    struct tinyc_cpp_context *ctx,
    const struct tinyc_repo *repo,
    struct tinyc_token *tokens,
    struct tinyc_token **out_tokens
) {
    assert(tokens && out_tokens);
    if (tinyc_token_is_punct_of(tokens, TINYC_TOKEN_PUNCT_SHARP)) {
        *out_tokens = NULL;  // No token produced by directive.
        struct tinyc_token *it = tokens;
        return parse_directive(ctx, repo, tokens, &it);
    } else {
        *out_tokens = tokens;
        return true;
    }
}
