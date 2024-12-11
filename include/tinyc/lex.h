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

#ifndef TINYC_LEX_H_
#define TINYC_LEX_H_

#include "tinyc/token.h"

struct tinyc_lex {};

/// Extract a token, return pointer to it.
/// Returns NULL if no token available.
struct tinyc_token_header *tinyc_lex_once(struct tinyc_lex *lex);

#endif  // TINYC_LEX_H_
