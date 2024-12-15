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

#ifndef TINYC_SOURCE_H_
#define TINYC_SOURCE_H_

#include <stdio.h>

#include "tinyc/string.h"

/// Physical source line.
struct tinyc_source_line {
    struct tinyc_source_line *next;  // NULL if no other line exists.
    struct tinyc_string line;
};

/// Arbitrary input source e.g. command line, include, etc.
///
/// The input is managed as physical lines: strings separated by '\n'.
/// Currently last characters doesn't end with newline is recognized as line.
struct tinyc_source {
    struct tinyc_string name;
    struct tinyc_source_line *lines;  // NULL if no line exists.
};

/// Construct source from string.
/// Returns false if failed.
bool tinyc_source_from_str(
    struct tinyc_source *this,
    const struct tinyc_string *name,
    const struct tinyc_string *content
);

/// Construct source from file stream.
/// Returns false if failed.
bool tinyc_source_from_fs(
    struct tinyc_source *this,
    const struct tinyc_string *name,
    FILE *fp
);

#endif  // TINYC_SOURCE_H_
