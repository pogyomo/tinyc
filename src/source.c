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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "tinyc/string.h"

static inline bool read(struct tinyc_string *content, FILE *fp) {
    char c;
    tinyc_string_init(content);
    while ((c = fgetc(fp)) != EOF) {
        tinyc_string_push(content, c);
    }
    return true;
}

static inline struct tinyc_source_line *extract_line(
    const struct tinyc_string *content,
    size_t *index
) {
    struct tinyc_source_line *line = malloc(sizeof(struct tinyc_source_line));
    if (!line) return NULL;
    line->next = NULL;
    tinyc_string_init(&line->line);
    while (*index < content->len) {
        char c = content->cstr[(*index)++];
        if (c == '\n') {
            break;
        } else {
            tinyc_string_push(&line->line, c);
        }
    }
    return line;
}

bool tinyc_source_from_str(
    struct tinyc_source *this,
    const struct tinyc_string *name,
    const struct tinyc_string *content
) {
    this->name = *name;
    this->lines = NULL;

    size_t index = 0;
    struct tinyc_source_line *last_line = this->lines;
    while (index < content->len) {
        struct tinyc_source_line *line = extract_line(content, &index);
        if (!line) return false;
        if (last_line) {
            last_line->next = line;
            last_line = line;
        } else {
            last_line = this->lines = line;
        }
    }
    return true;
}

bool tinyc_source_from_fs(
    struct tinyc_source *this,
    const struct tinyc_string *name,
    FILE *fp
) {
    struct tinyc_string content;
    if (read(&content, fp)) {
        return tinyc_source_from_str(this, name, &content);
    } else {
        return false;
    }
}
