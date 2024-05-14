#ifndef TINYC_REPORT_H_
#define TINYC_REPORT_H_

#include "collections/string.h"
#include "context.h"
#include "span.h"

typedef enum {
    REPORT_LEVEL_ERROR,
    REPORT_LEVEL_WARNING,
} report_level_t;

typedef struct {
    string_t *what;
    string_t *info;
    span_t span;
} report_info_t;

report_info_t report_info_make(string_t *what, string_t *info, span_t span);

void report(context_t *ctx, report_level_t level, report_info_t info);

#endif  // TINYC_REPORT_H_
