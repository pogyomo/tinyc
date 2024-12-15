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

#include "tinyc/diag.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "tinyc/repo.h"
#include "tinyc/source.h"

static inline void validate_span(const struct tinyc_span *span) {
    assert(span->start.row <= span->end.row);
    if (span->start.row == span->end.row) {
        assert(span->start.offset <= span->end.offset);
    }
}

static inline const char *severity_string(enum tinyc_diag_severity severity) {
    switch (severity) {
        case TINYC_DIAG_INFO:
            return "info";
        case TINYC_DIAG_WARN:
            return "warning";
        default:  // TINYC_DIAG_ERROR
            return "error";
    }
}

static inline const char *color_sequence_by_severity(
    enum tinyc_diag_severity severity
) {
    switch (severity) {
        case TINYC_DIAG_INFO:
            return "\033[34m";
        case TINYC_DIAG_WARN:
            return "\033[33m";
        default:  // TINYC_DIAG_ERROR
            return "\033[31m";
    }
}

static inline void paint(
    FILE *fs,
    bool color,
    enum tinyc_diag_severity severity,
    const char *s
) {
    if (color) fputs(color_sequence_by_severity(severity), fs);
    fputs(s, fs);
    if (color) fputs("\033[0m", fs);
}

static inline void emit_loc_info(
    FILE *fs,
    bool color,
    const struct tinyc_source *source,
    enum tinyc_diag_severity severity,
    const struct tinyc_span *span,
    const struct tinyc_string *what
) {
    const size_t row = span->start.row;
    const size_t offset = span->start.offset;

    fprintf(fs, "%s:%ld:%ld: ", source->name.cstr, row, offset);
    paint(fs, color, severity, severity_string(severity));
    paint(fs, color, severity, ":");
    fprintf(fs, " %s", what->cstr);
}

static inline void emit_start_line(
    FILE *fs,
    const struct tinyc_source *source,
    const struct tinyc_span *span,
    const struct tinyc_string *message
) {
    const struct tinyc_source_line *line = tinyc_source_at(
        source,
        span->start.row
    );
    assert(line);

    fprintf(fs, " %5ld | %s\n", span->start.row, line->line.cstr);
    fputs("       | ", fs);
    for (size_t i = 0; i < span->start.offset; ++i) fputc(' ', fs);
    for (size_t i = span->start.offset; i < line->line.len; ++i) fputc('^', fs);
}

static inline void emit_end_line(
    FILE *fs,
    const struct tinyc_source *source,
    const struct tinyc_span *span,
    const struct tinyc_string *message
) {
    const struct tinyc_source_line *line = tinyc_source_at(
        source,
        span->end.row
    );
    assert(line);

    fprintf(fs, " %5ld | %s\n", span->end.row, line->line.cstr);
    fputs("       | ", fs);
    for (size_t i = 0; i <= span->end.offset; ++i) fputc('^', fs);
}

static inline void emit_single_line(
    FILE *fs,
    const struct tinyc_source *source,
    const struct tinyc_span *span
) {
    const struct tinyc_source_line *line = tinyc_source_at(
        source,
        span->start.row
    );
    assert(line);

    fprintf(fs, " %5ld | %s\n", span->start.row, line->line.cstr);
    fputs("       | ", fs);
    for (size_t i = 0; i < span->start.offset; ++i) fputc(' ', fs);
    for (size_t i = span->start.offset; i <= span->end.offset; ++i) {
        fputc('^', fs);
    }
}

static inline void diagnostic_line(
    FILE *fs,
    bool color,
    enum tinyc_diag_severity severity,
    const struct tinyc_repo *repo,
    const struct tinyc_span *span,
    const struct tinyc_string *what,
    const struct tinyc_string *message
) {
    const struct tinyc_source *source = tinyc_repo_query(repo, span->id);
    assert(source);

    emit_loc_info(fs, color, source, severity, span, what);
    fputc('\n', fs);
    emit_single_line(fs, source, span);
    fprintf(fs, " %s\n", message->cstr);
}

static inline void diagnostic_lines(
    FILE *fs,
    bool color,
    enum tinyc_diag_severity severity,
    const struct tinyc_repo *repo,
    const struct tinyc_span *span,
    const struct tinyc_string *what,
    const struct tinyc_string *message
) {
    const struct tinyc_source *source = tinyc_repo_query(repo, span->id);
    assert(source);

    emit_loc_info(fs, color, source, severity, span, what);
    fputc('\n', fs);
    emit_start_line(fs, source, span, message);
    fputc('\n', fs);
    emit_end_line(fs, source, span, message);
    fprintf(fs, " %s\n", message->cstr);
}

static void diagnostic(
    FILE *fs,
    bool color,
    enum tinyc_diag_severity severity,
    const struct tinyc_repo *repo,
    const struct tinyc_span *span,
    const struct tinyc_string *what,
    const struct tinyc_string *message
) {
    validate_span(span);
    if (span->start.row == span->end.row) {
        diagnostic_line(fs, color, severity, repo, span, what, message);
    } else {
        diagnostic_lines(fs, color, severity, repo, span, what, message);
    }
}

void tinyc_diag(
    enum tinyc_diag_severity severity,
    const struct tinyc_repo *repo,
    const struct tinyc_span *span,
    const struct tinyc_string *what,
    const struct tinyc_string *message
) {
    diagnostic(stdout, true, severity, repo, span, what, message);
}

void tinyc_diag_fs(
    FILE *fs,
    enum tinyc_diag_severity severity,
    const struct tinyc_repo *repo,
    const struct tinyc_span *span,
    const struct tinyc_string *what,
    const struct tinyc_string *message
) {
    diagnostic(fs, false, severity, repo, span, what, message);
}
