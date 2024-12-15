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

#ifndef TINYC_CPP_H_
#define TINYC_CPP_H_

#include "tinyc/repo.h"
#include "tinyc/token.h"

struct tinyc_cpp_context {
    int if_count;  // > 0 if inside if.
};

/// Do preprocess for tokens in a line.
/// Returns NULL if failed.
struct tinyc_token *tinyc_cpp(
    struct tinyc_cpp_context *ctx,
    const struct tinyc_repo *repo,
    struct tinyc_token *tokens
);

#endif  // TINYC_CPP_H_
