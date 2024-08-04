// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_REPORT_H_
#define TINYC_REPORT_H_

#include "context.h"
#include "span.h"

enum report_kind {
    REPORT_ERROR,
    REPORT_WARN,
    REPORT_INFO,
};

struct report_info {
    enum report_kind kind;
    struct span span;
    char *what;
    char *detail;
};

void report(struct context *ctx, struct report_info *info);

#endif  // TINYC_REPORT_H_
