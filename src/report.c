#include "report.h"

#include <stdint.h>
#include <stdio.h>

#include "context.h"
#include "stdio.h"
#include "string.h"
#include "vector.h"

static uint8_t width(size_t num) {
    uint8_t width = 0;
    while (num) {
        num /= 10;
        width += 1;
    }
    return width;
}

static uint8_t max(uint8_t l, uint8_t r) { return l > r ? l : r; }

static void put_kind(FILE *os, enum report_kind kind) {
    if (kind == REPORT_ERROR) {
        fputs("\e[31merror: \e[0m", os);
    } else if (kind == REPORT_WARN) {
        fputs("\e[33mwarning: \e[0m", os);
    } else {
        fputs("\e[34minfo: \e[0m", os);
    }
}

static void end_color() { fputs("\e[0m", stdout); }

static void splitline(const struct string *s, struct vector *lines) {
    vector_init(lines, sizeof(struct string));

    struct string line;
    string_init(&line);
    for (size_t i = 0; i < s->len; i++) {
        char c = string_at(s, i);
        if (c == '\n') {
            vector_push(lines, &line);
            string_init(&line);
        } else {
            string_push(&line, c);
        }
    }
}

void report(struct context *ctx, struct report_info *info) {
    const struct cache_entry *entry = context_fetch(ctx, info->span.id);

    struct position *start = &info->span.start;
    struct position *end = &info->span.end;

    struct vector lines;
    splitline(&entry->content, &lines);

    fprintf(stderr, "%s:%zu:%zu: ", entry->name, start->row, start->col);
    put_kind(stderr, info->kind);
    fprintf(stderr, "%s\n", info->what);

    size_t vis_start_row = start->row + 1;
    size_t vis_end_row = end->row + 1;
    uint8_t row_width = max(width(vis_start_row), width(vis_end_row));
    if (info->span.start.row == info->span.end.row) {
        struct string *line = vector_at(&lines, start->row);
        for (uint8_t i = 0; i < row_width - width(vis_start_row); i++)
            fputc(' ', stderr);
        fprintf(stderr, " %zu|%s\n", vis_start_row, line->str);

        for (uint8_t i = 0; i < row_width + 1; i++) fputc(' ', stderr);
        fputc('|', stderr);
        for (size_t i = 0; i < start->col; i++) fputc(' ', stderr);
        fputc('^', stderr);
        for (size_t i = start->col + 1; i <= end->col; i++) fputc('~', stderr);
        fprintf(stderr, " %s\n", info->detail);
    } else {
        struct string *start_line = vector_at(&lines, start->row);
        for (uint8_t i = 0; i < row_width - width(vis_start_row); i++)
            fputc(' ', stderr);
        fprintf(stderr, " %zu|%s\n", vis_start_row, start_line->str);

        for (uint8_t i = 0; i < row_width + 1; i++) fputc(' ', stderr);
        fputc('|', stderr);
        for (size_t i = 0; i < start->col; i++) fputc(' ', stderr);
        fputc('^', stderr);
        for (size_t i = start->col + 1; i < start_line->len; i++)
            fputc('~', stderr);
        fputc('\n', stderr);

        for (uint8_t i = 0; i < row_width; i++) fputc(' ', stderr);
        fputs(" :\n", stderr);

        struct string *end_line = vector_at(&lines, start->row);
        for (uint8_t i = 0; i < row_width - width(vis_end_row); i++)
            fputc(' ', stderr);
        fprintf(stderr, " %zu|%s\n", vis_end_row, end_line->str);

        for (uint8_t i = 0; i < row_width + 1; i++) fputc(' ', stderr);
        fputc('|', stderr);
        for (size_t i = 0; i < start->col; i++) fputc(' ', stderr);
        fputc('^', stderr);
        for (size_t i = 0; i < end->col; i++) fputc('~', stderr);
        fprintf(stderr, " %s\n", info->detail);
    }
}
