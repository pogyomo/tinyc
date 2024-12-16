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

#ifndef TINYC_CPP_HELPER_H_
#define TINYC_CPP_HELPER_H_

#include <stdbool.h>

#include "tinyc/repo.h"
#include "tinyc/token.h"

/// Expect token after it, advance it.
/// Emit diagnostic if not.
bool tinyc_cpp_expect_token_next(
    const struct tinyc_repo *repo,
    struct tinyc_token *head,
    struct tinyc_token **it
);

/// Expect token to be punctuation.
/// Emit diagnostic if not.
bool tinyc_cpp_expect_punct_of(
    const struct tinyc_repo *repo,
    struct tinyc_token *token,
    enum tinyc_token_punct_kind kind
);

/// Expect token to be identifer.
/// Emit diagnostic if not.
bool tinyc_cpp_expect_ident(
    const struct tinyc_repo *repo,
    struct tinyc_token *token
);

#endif  // TINYC_CPP_HELPER_H_
