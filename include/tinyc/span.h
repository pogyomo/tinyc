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

#ifndef TINYC_SPAN_H_
#define TINYC_SPAN_H_

#include <stddef.h>

#include "tinyc/repo.h"

/// Position in source code.
struct tinyc_position {
    size_t row, offset;  // 0-indexed.
};

/// Represent a range of source code.
struct tinyc_span {
    tinyc_repo_id id;
    struct tinyc_position start, end;
};

/// Add two span, returns it.
void tinyc_span_add(
    const struct tinyc_span *s1,
    const struct tinyc_span *s2,
    struct tinyc_span *res
);

#endif  // TINYC_SPAN_H_
