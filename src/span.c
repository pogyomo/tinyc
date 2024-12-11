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

#include "tinyc/span.h"

static inline size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

static inline size_t max(size_t a, size_t b) {
    return a > b ? a : b;
}

struct tinyc_span tinyc_span_add(struct tinyc_span s1, struct tinyc_span s2) {
    struct tinyc_position start = {
        min(s1.start.row, s2.start.row),
        min(s1.start.offset, s1.start.offset),
    };
    struct tinyc_position end = {
        max(s1.end.row, s2.end.row),
        max(s1.end.offset, s1.end.offset),
    };
    return (struct tinyc_span){start, end};
}
