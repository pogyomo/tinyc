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
#include <tinyc/source.h>

#include "tinyc/string.h"

static inline bool check_lines(
    struct tinyc_source *source,
    size_t n,
    char *lines[n]
) {
    struct tinyc_source_line *line = source->lines;
    for (size_t i = 0; i < n; ++i, line = line->next) {
        struct tinyc_string expect_line;
        tinyc_string_from(&expect_line, lines[i]);

        if (!line || tinyc_string_cmp(&line->line, &expect_line) != 0) {
            return false;
        }
    }
    assert(!line);
    return true;
}

static inline bool check_line(
    const struct tinyc_source_line *line,
    char *expect_line
) {
    struct tinyc_string expect_line_;
    tinyc_string_from(&expect_line_, expect_line);
    return tinyc_string_cmp(&line->line, &expect_line_) == 0;
}

static void init_from_str(void) {
    struct tinyc_string name, content;
    tinyc_string_from(&name, "name");
    tinyc_string_from(&content, "line1\nline2\nline3\n");

    struct tinyc_source source;
    assert(tinyc_source_from_str(&source, &name, &content));
    assert(tinyc_string_cmp(&source.name, &name) == 0);
    assert(check_lines(&source, 3, (char *[3]){"line1", "line2", "line3"}));
}

static void init_from_file(void) {
    struct tinyc_string name, content;
    tinyc_string_from(&name, "name");
    tinyc_string_from(&content, "line1\nline2\nline3\n");

    FILE *fp = tmpfile();
    assert(fp);
    for (size_t i = 0; i < content.len; i++) {
        fputc(content.cstr[i], fp);
    }
    rewind(fp);

    struct tinyc_source source;
    assert(tinyc_source_from_fs(&source, &name, fp));
    assert(tinyc_string_cmp(&source.name, &name) == 0);
    assert(check_lines(&source, 3, (char *[3]){"line1", "line2", "line3"}));

    fclose(fp);
}

static void missing_tail_newline(void) {
    struct tinyc_string name, content;
    tinyc_string_from(&name, "name");
    tinyc_string_from(&content, "line1\nline2\nline3");

    struct tinyc_source source;
    assert(tinyc_source_from_str(&source, &name, &content));
    assert(tinyc_string_cmp(&source.name, &name) == 0);
    assert(check_lines(&source, 3, (char *[3]){"line1", "line2", "line3"}));
}

static void empty_source(void) {
    struct tinyc_string name, content;
    tinyc_string_from(&name, "name");
    tinyc_string_from(&content, "");

    struct tinyc_source source;
    assert(tinyc_source_from_str(&source, &name, &content));
    assert(tinyc_string_cmp(&source.name, &name) == 0);
    assert(source.lines == NULL);
}

static void with_empty_line(void) {
    struct tinyc_string name, content;
    tinyc_string_from(&name, "name");
    tinyc_string_from(&content, "line1\n\nline3");

    struct tinyc_source source;
    assert(tinyc_source_from_str(&source, &name, &content));
    assert(tinyc_string_cmp(&source.name, &name) == 0);
    assert(check_lines(&source, 3, (char *[3]){"line1", "", "line3"}));
}

static void lines_at(void) {
    struct tinyc_string name, content;
    tinyc_string_from(&name, "name");
    tinyc_string_from(&content, "line1\nline2\nline3");

    struct tinyc_source source;
    assert(tinyc_source_from_str(&source, &name, &content));
    const struct tinyc_source_line *line1 = tinyc_source_at(&source, 0);
    assert(line1 && check_line(line1, "line1"));
    const struct tinyc_source_line *line2 = tinyc_source_at(&source, 1);
    assert(line2 && check_line(line2, "line2"));
    const struct tinyc_source_line *line3 = tinyc_source_at(&source, 2);
    assert(line3 && check_line(line3, "line3"));
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
