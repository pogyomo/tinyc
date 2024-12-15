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
#include <stdio.h>
#include <string.h>
#include <tinyc/source.h>

#include "tinyc/string.h"

static inline bool check_lines(
    struct tinyc_source *source,
    size_t n,
    char *lines[n]
) {
    struct tinyc_source_line *line = source->lines;
    for (size_t i = 0; i < n; ++i, line = line->next) {
        if (!line || strcmp(line->line.cstr, lines[i]) != 0) {
            return false;
        }
    }
    return !line;
}

static void init_from_str(void) {
    struct tinyc_source source;
    assert(tinyc_source_from_str(&source, "name", "line1\nline2\nline3\n"));
    assert(strcmp(source.name.cstr, "name") == 0);
    assert(check_lines(&source, 3, (char *[3]){"line1", "line2", "line3"}));
}

static void init_from_file(void) {
    FILE *fp = tmpfile();
    assert(fp);
    fputs("line1\nline2\nline3\n", fp);
    rewind(fp);

    struct tinyc_source source;
    assert(tinyc_source_from_fs(&source, "name", fp));
    assert(strcmp(source.name.cstr, "name") == 0);
    assert(check_lines(&source, 3, (char *[3]){"line1", "line2", "line3"}));

    fclose(fp);
}

static void missing_tail_newline(void) {
    struct tinyc_source source;
    assert(tinyc_source_from_str(&source, "name", "line1\nline2\nline3"));
    assert(strcmp(source.name.cstr, "name") == 0);
    assert(check_lines(&source, 3, (char *[3]){"line1", "line2", "line3"}));
}

static void empty_source(void) {
    struct tinyc_string name, content;
    tinyc_string_from(&name, "name");
    tinyc_string_from(&content, "");

    struct tinyc_source source;
    assert(tinyc_source_from_str(&source, "name", ""));
    assert(strcmp(source.name.cstr, "name") == 0);
    assert(source.lines == NULL);
}

static void with_empty_line(void) {
    struct tinyc_source source;
    assert(tinyc_source_from_str(&source, "name", "line1\n\nline3"));
    assert(strcmp(source.name.cstr, "name") == 0);
    assert(check_lines(&source, 3, (char *[3]){"line1", "", "line3"}));
}

static void lines_at(void) {
    struct tinyc_source source;
    assert(tinyc_source_from_str(&source, "name", "line1\nline2\nline3"));
    const struct tinyc_source_line *line1 = tinyc_source_at(&source, 0);
    assert(line1 && strcmp(line1->line.cstr, "line1") == 0);
    const struct tinyc_source_line *line2 = tinyc_source_at(&source, 1);
    assert(line2 && strcmp(line2->line.cstr, "line2") == 0);
    const struct tinyc_source_line *line3 = tinyc_source_at(&source, 2);
    assert(line3 && strcmp(line3->line.cstr, "line3") == 0);
    const struct tinyc_source_line *line4 = tinyc_source_at(&source, 3);
    assert(!line4);
}

int main(void) {
    init_from_str();
    init_from_file();
    missing_tail_newline();
    empty_source();
    with_empty_line();
    lines_at();
}
