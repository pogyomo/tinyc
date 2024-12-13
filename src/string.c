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

#include "tinyc/string.h"

#include <stdlib.h>
#include <string.h>

#define DEFAULT_CAP 100

static inline size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

static inline bool copy_alloc(struct tinyc_string *this) {
    const char *from = this->cstr;
    const size_t new_cap = this->len + 1 + DEFAULT_CAP;

    char *new_cstr = realloc(this->cstr, sizeof(char) * new_cap);
    if (!new_cstr) return false;

    this->cstr = new_cstr;
    this->cap = new_cap;

    strcpy(this->cstr, from);
    return true;
}

static inline bool extend(struct tinyc_string *this) {
    const char *from = this->cstr;
    this->cap = this->len + 1 + DEFAULT_CAP;
    this->cstr = malloc(sizeof(char) * this->cap);
    if (!this->cstr) return false;
    strcpy(this->cstr, from);
    return true;
}

/// Returns true if string with cap is containable string with len.
static inline bool containable_len(size_t cap, size_t len) {
    return cap >= len + 1;
}

bool tinyc_string_init(struct tinyc_string *this) {
    this->cap = DEFAULT_CAP;
    this->len = 0;
    this->cstr = malloc(sizeof(char) * this->cap);
    if (!this->cstr) return NULL;
    this->cstr[0] = '\0';
    return true;
}

bool tinyc_string_from(struct tinyc_string *this, char *from) {
    this->cap = 0;
    this->len = strlen(from);
    this->cstr = from;
    return true;
}

bool tinyc_string_push(struct tinyc_string *this, char c) {
    if (this->cap == 0 && !copy_alloc(this)) return false;
    if (!containable_len(this->cap, this->len) && !extend(this)) return false;
    this->cstr[this->len++] = c;
    this->cstr[this->len] = '\0';
    return true;
}

int tinyc_string_cmp(struct tinyc_string s1, struct tinyc_string s2) {
    for (size_t i = 0; i < min(s1.len, s2.len); ++i) {
        if (s1.cstr[i] < s2.cstr[i]) {
            return -1;
        } else if (s1.cstr[i] > s2.cstr[i]) {
            return 1;
        }
    }
    if (s1.len < s2.len) {
        return -1;
    } else if (s1.len > s2.len) {
        return 1;
    } else {
        return 0;
    }
}
