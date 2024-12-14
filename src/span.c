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

static inline void set_start(
    const struct tinyc_span *s1,
    const struct tinyc_span *s2,
    struct tinyc_span *res
) {
    if (s1->start.row < s2->start.row) {
        res->start = s1->start;
    } else if (s1->start.row > s2->start.row) {
        res->start = s2->start;
    } else {
        if (s1->start.offset < s2->start.offset) {
            res->start = s1->start;
        } else {
            res->start = s2->start;
        }
    }
}

static inline void set_end(
    const struct tinyc_span *s1,
    const struct tinyc_span *s2,
    struct tinyc_span *res
) {
    if (s1->end.row < s2->end.row) {
        res->end = s2->end;
    } else if (s1->end.row > s2->end.row) {
        res->end = s1->end;
    } else {
        if (s1->end.offset < s2->end.offset) {
            res->end = s2->end;
        } else {
            res->end = s1->end;
        }
    }
}

void tinyc_span_add(
    const struct tinyc_span *s1,
    const struct tinyc_span *s2,
    struct tinyc_span *res
) {
    set_start(s1, s2, res);
    set_end(s1, s2, res);
}
