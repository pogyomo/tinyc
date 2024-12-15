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

#ifndef TINYC_STRING_H_
#define TINYC_STRING_H_

#include <stdbool.h>
#include <stddef.h>

/// A variable length string.
struct tinyc_string {
    size_t cap;  // Capacity of cstr, or 0 for static string.
    size_t len;  // Length of cstr. Doesn't include '\0'.
    char *cstr;  // String terminated with '\0'.
};

/// Initialize string with default capacity.
/// Returns false if initialization failed.
bool tinyc_string_init(struct tinyc_string *this);

/// Initialize string from string.
/// Returns false if initialization failed.
bool tinyc_string_from(struct tinyc_string *this, char *from);

/// Initialize string from string.
/// Unlike tinyc_string_from, it copy from and allocate memory for it.
/// Returns false if initialization failed.
bool tinyc_string_from_copy(struct tinyc_string *this, const char *from);

/// Push character to string.
/// Returns false if operation failed.
bool tinyc_string_push(struct tinyc_string *this, char c);

/// Compare two string. Semantics is same as strcmp.
int tinyc_string_cmp(
    const struct tinyc_string *s1,
    const struct tinyc_string *s2
);

#endif  // TINYC_STRING_H_
