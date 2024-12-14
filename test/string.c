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
#include <tinyc/string.h>

static void test_tinyc_string_from(void) {
    struct tinyc_string s;
    tinyc_string_from(&s, "hello");

    assert(s.len == 5);
    assert(s.cstr[0] == 'h');
    assert(s.cstr[1] == 'e');
    assert(s.cstr[2] == 'l');
    assert(s.cstr[3] == 'l');
    assert(s.cstr[4] == 'o');
}

static void test_tinyc_string_push(void) {
    struct tinyc_string s;
    tinyc_string_init(&s);

    tinyc_string_push(&s, 'h');
    tinyc_string_push(&s, 'e');
    tinyc_string_push(&s, 'l');
    tinyc_string_push(&s, 'l');
    tinyc_string_push(&s, 'o');

    assert(s.len == 5);
    assert(s.cstr[0] == 'h');
    assert(s.cstr[1] == 'e');
    assert(s.cstr[2] == 'l');
    assert(s.cstr[3] == 'l');
    assert(s.cstr[4] == 'o');
}

static void test_tinyc_string_push_extend(void) {
    struct tinyc_string s;
    tinyc_string_init(&s);
    const size_t init_cap = s.cap;

    size_t count;
    for (count = 0; init_cap == s.cap; count++) {
        tinyc_string_push(&s, 'a');
    }

    for (size_t i = 0; i < count; i++) {
        assert(s.cstr[i] == 'a');
    }
    assert(s.len == count);
    assert(s.cstr[count] == '\0');
}

static void test_tinyc_string_cmp_l(void) {
    struct tinyc_string s1, s2;
    tinyc_string_from(&s1, "aaa");
    tinyc_string_from(&s2, "aab");
    assert(tinyc_string_cmp(&s1, &s2) < 0);
}

static void test_tinyc_string_cmp_g(void) {
    struct tinyc_string s1, s2;
    tinyc_string_from(&s1, "aab");
    tinyc_string_from(&s2, "aaa");
    assert(tinyc_string_cmp(&s1, &s2) > 0);
}

static void test_tinyc_string_cmp_e(void) {
    struct tinyc_string s1, s2;
    tinyc_string_from(&s1, "aaa");
    tinyc_string_from(&s2, "aaa");
    assert(tinyc_string_cmp(&s1, &s2) == 0);
}

int main(void) {
    test_tinyc_string_from();
    test_tinyc_string_push();
    test_tinyc_string_push_extend();
    test_tinyc_string_cmp_l();
    test_tinyc_string_cmp_g();
    test_tinyc_string_cmp_e();
}
