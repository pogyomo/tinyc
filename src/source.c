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
#include <string.h>

#include "tinyc/string.h"

/// Character reader read from either file stream or string.
struct reader {
    FILE *fs;       // If non-null, read character from file stream.
    const char *s;  // if non-null, read character from string.
    size_t index;
    size_t len;
};

static inline int read(struct reader *this) {
    if (this->fs) {
        return fgetc(this->fs);
    } else {
        if (this->index < this->len) {
            return this->s[this->index++];
        } else {
            return EOF;
        }
    }
}

static inline char eof(struct reader *this) {
    if (this->fs) {
        return feof(this->fs);
    } else {
        return this->index >= this->len;
    }
}

static inline struct tinyc_source_line *extract_line(struct reader *reader) {
    struct tinyc_source_line *line = malloc(sizeof(struct tinyc_source_line));
    if (!line) return NULL;
    line->next = NULL;
    tinyc_string_init(&line->line);

    char c;
    while (!eof(reader)) {
        c = read(reader);
        if (c == '\n') {
            break;
        } else {
            tinyc_string_push(&line->line, c);
        }
    }
    return line;
}

static inline bool read_lines(
    struct tinyc_source *this,
    const char *name,
    struct reader *reader
) {
    tinyc_string_from_copy(&this->name, name);
    this->lines = NULL;

    struct tinyc_source_line *last_line = this->lines;
    while (!eof(reader)) {
        struct tinyc_source_line *line = extract_line(reader);
        if (!line) return false;
        if (last_line) {
            last_line->next = line;
        } else {
            this->lines = last_line = line;
        }
        line->next = NULL;
    }
    return true;
}

bool tinyc_source_from_str(
    struct tinyc_source *this,
    const char *name,
    const char *content
) {
    struct reader reader = {NULL, content, 0, strlen(content)};
    return read_lines(this, name, &reader);
}

bool tinyc_source_from_fs(
    struct tinyc_source *this,
    const char *name,
    FILE *fs
) {
    struct reader reader = {fs, NULL, 0, 0};
    return read_lines(this, name, &reader);
}

const struct tinyc_source_line *tinyc_source_at(
    const struct tinyc_source *this,
    size_t n
) {
    struct tinyc_source_line *line = this->lines;
    if (!line) return NULL;
    for (size_t i = 0; i < n; ++i) {
        line = line->next;
        if (!line) return NULL;
    }
    return line;
}
