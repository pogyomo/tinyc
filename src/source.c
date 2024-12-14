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

#include "tinyc/source.h"

#include <stdio.h>

#include "tinyc/string.h"

static inline bool read(struct tinyc_string *content, FILE *fp) {
    char c;
    tinyc_string_init(content);
    while ((c = fgetc(fp)) != EOF) {
        tinyc_string_push(content, c);
    }
    return true;
}

bool tinyc_source_from_fs(
    struct tinyc_source *this,
    const struct tinyc_string *name,
    FILE *fp
) {
    this->name = *name;
    return read(&this->content, fp);
}
