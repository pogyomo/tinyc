#include "utils.h"

#include <stdarg.h>

#include "../report.h"

bool check_eos(context_t *ctx, tstream_t *ts) {
    if (tstream_eos(ts)) {
        string_t what, info;
        string_from(&what, "expected token after this");
        string_init(&info);
        report(ctx, REPORT_LEVEL_ERROR,
               (report_info_t){what, info, tstream_last(ts)->span});
        return false;
    } else {
        return true;
    }
}

bool check_kind(context_t *ctx, tstream_t *ts, token_kind_t kind) {
    TRY(check_eos(ctx, ts));
    if (tstream_token(ts)->kind != kind) {
        string_t what, info;
        string_format(&what, "expected this to be %s",
                      token_kind_to_string(kind));
        string_init(&info);
        report(ctx, REPORT_LEVEL_ERROR,
               (report_info_t){what, info, tstream_token(ts)->span});
        return false;
    } else {
        return true;
    }
}

bool check_kinds(context_t *ctx, tstream_t *ts, size_t n, ...) {
    TRY(check_eos(ctx, ts));
    token_kind_t target = tstream_token(ts)->kind;

    string_t what, info;
    token_kind_t kind;
    string_from(&what, "expected this to be one of ");
    string_init(&info);

    va_list ap;
    va_start(ap, n);
    for (size_t i = 0; i < n; i++) {
        kind = va_arg(ap, token_kind_t);
        if (target == kind) {
            va_end(ap);
            return true;
        }
        string_append(&what, token_kind_to_string(kind));
        if (n >= 2) {
            if (i == n - 2) {
                string_append(&what, " and ");
            } else if (i < n - 2) {
                string_append(&what, ", ");
            }
        }
    }
    va_end(ap);

    report(ctx, REPORT_LEVEL_ERROR,
           (report_info_t){what, info, tstream_token(ts)->span});
    return false;
}

bool expect_kind(context_t *ctx, tstream_t *ts, token_kind_t kind,
                 span_t *span) {
    TRY(check_kind(ctx, ts, kind));
    if (span != NULL) *span = tstream_token(ts)->span;
    tstream_advance(ts);
    return true;
}
