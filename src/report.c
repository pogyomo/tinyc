#include "report.h"

#include <stdio.h>

#include "cache.h"
#include "collections/string.h"
#include "context.h"
#include "span.h"

// Start to coloring the output to `os` depende on `level`.
static void start_color(FILE *os, report_level_t level) {
    if (level == REPORT_LEVEL_ERROR) {
        fputs("\e[31m", os);
    } else {
        fputs("\e[32m", os);
    }
}

// Stop to coloring the output to `os`.
static void end_color(FILE *os) { fputs("\e[0m", os); }

// Returns the number of digits in `n`.
static int digits(int n) {
    if (n == 0) return 1;
    int res = 0;
    while (n > 0) {
        res++;
        n /= 10;
    }
    return res;
}

void report(context_t *ctx, report_level_t level, report_info_t info) {
    input_t *input = cache_fetch(&ctx->cache, info.span.id);
    position_t start = info.span.start;
    position_t end = info.span.end;

    fprintf(stderr, "%s:%d:%d: ", input->name.str, start.row, start.offset);
    start_color(stderr, level);
    if (level == REPORT_LEVEL_ERROR) {
        fputs("error: ", stderr);
    } else {
        fputs("warning: ", stderr);
    }
    end_color(stderr);
    fprintf(stderr, "%s\n", info.what.str);

    int row_width = digits(start.row) > digits(end.row) ? digits(start.row)
                                                        : digits(end.row);
    if (start.row == end.row) {
        string_t *line = input_at(input, start.row);
        fprintf(stderr, "  %d|%s\n", start.row, line->str);
        for (int i = 0; i < 2 + row_width; i++) fputc(' ', stderr);
        fputc('|', stderr);
        for (int i = 0; i < start.offset; i++) fputc(' ', stderr);
        start_color(stderr, level);
        fputc('^', stderr);
        for (int i = start.offset + 1; i <= end.offset; i++) fputc('~', stderr);
        end_color(stderr);
        fprintf(stderr, " %s\n", info.info.str);
    } else {
        string_t *sline = input_at(input, start.row);
        fputs("  ", stderr);
        for (int i = 0; i < row_width - digits(start.row); i++)
            fputc('0', stderr);
        fprintf(stderr, "%d|%s\n", start.row, sline->str);
        for (int i = 0; i < 2 + row_width; i++) fputc(' ', stderr);
        fputc('|', stderr);
        for (int i = 0; i < start.offset; i++) fputc(' ', stderr);
        start_color(stderr, level);
        fputc('^', stderr);
        for (int i = start.offset + 1; i < sline->len; i++) fputc('~', stderr);
        end_color(stderr);
        fputc('\n', stderr);

        fputs("  ", stderr);
        for (int i = 0; i < row_width; i++) fputc(' ', stderr);
        fputs(":\n", stderr);

        string_t *eline = input_at(input, end.row);
        fputs("  ", stderr);
        for (int i = 0; i < row_width - digits(end.row); i++)
            fputc('0', stderr);
        fprintf(stderr, "%d|%s\n", end.row, eline->str);
        for (int i = 0; i < 2 + row_width; i++) fputc(' ', stderr);
        fputc('|', stderr);
        start_color(stderr, level);
        for (int i = 0; i <= end.offset; i++) fputc('~', stderr);
        end_color(stderr);
        fprintf(stderr, " %s\n", info.info.str);
    }
}
