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

#include <assert.h>
#include <tinyc/span.h>

static void add_cross(void) {
    struct tinyc_span s1 = {
        0,
        {1, 10},
        {3, 5 }
    };
    struct tinyc_span s2 = {
        0,
        {2, 8},
        {4, 5}
    };
    struct tinyc_span s;

    tinyc_span_add(&s1, &s2, &s);
    assert(s.start.row == 1);
    assert(s.start.offset == 10);
    assert(s.end.row == 4);
    assert(s.end.offset == 5);

    tinyc_span_add(&s2, &s1, &s);
    assert(s.start.row == 1);
    assert(s.start.offset == 10);
    assert(s.end.row == 4);
    assert(s.end.offset == 5);
}

static void add_surround(void) {
    struct tinyc_span s1 = {
        0,
        {1, 10},
        {4, 8 }
    };
    struct tinyc_span s2 = {
        0,
        {2, 8},
        {3, 5}
    };
    struct tinyc_span s;

    tinyc_span_add(&s1, &s2, &s);
    assert(s.start.row == s1.start.row);
    assert(s.start.offset == s1.start.offset);
    assert(s.end.row == s1.end.row);
    assert(s.end.offset == s1.end.offset);

    tinyc_span_add(&s2, &s1, &s);
    assert(s.start.row == s1.start.row);
    assert(s.start.offset == s1.start.offset);
    assert(s.end.row == s1.end.row);
    assert(s.end.offset == s1.end.offset);
}

static void add_same(void) {
    struct tinyc_span s1 = {
        0,
        {1, 10},
        {4, 8 }
    };
    struct tinyc_span s;

    tinyc_span_add(&s1, &s1, &s);
    assert(s.start.row == s1.start.row);
    assert(s.start.offset == s1.start.offset);
    assert(s.end.row == s1.end.row);
    assert(s.end.offset == s1.end.offset);
}

int main(void) {
    add_cross();
    add_surround();
    add_same();
}
