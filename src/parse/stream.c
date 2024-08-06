#include "stream.h"

#include <assert.h>
#include <stdio.h>

#include "../panic.h"
#include "../report.h"

void tstream_init(struct tstream *ts, struct token *tokens) {
    assert(tokens);
    ts->curr = tokens;
    ts->last = tokens;
}

bool tstream_eos(struct tstream *ts) { return ts->curr == NULL; }

struct token *tstream_curr(struct tstream *ts) {
    if (tstream_eos(ts)) fatal_error("used eos tstream");
    return ts->curr;
}

struct token *tstream_last(struct tstream *ts) { return ts->last; }

void tstream_advance(struct tstream *ts) {
    if (tstream_eos(ts)) return;
    ts->last = ts->curr;
    ts->curr = ts->curr->next;
}

bool tstream_check_eos(struct context *ctx, struct tstream *ts) {
    if (tstream_eos(ts)) {
        struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                   "expected token after this", ""};
        report(ctx, &info);
        return false;
    } else {
        return true;
    }
}

bool tstream_check_punct(struct context *ctx, struct tstream *ts,
                         enum token_punct_kind kind) {
    if (!tstream_is_punct(ts, kind)) {
        char what[500];
        snprintf(what, 500, "expected `%s` after this",
                 token_punct_kind_to_str(kind));
        struct report_info info = {REPORT_ERROR, tstream_last(ts)->span, what,
                                   ""};
        report(ctx, &info);
        return false;
    } else {
        return true;
    }
}

bool tstream_check_keyword(struct context *ctx, struct tstream *ts,
                           enum token_keyword_kind kind) {
    if (!tstream_is_keyword(ts, kind)) {
        char what[500];
        snprintf(what, 500, "expected `%s` after this",
                 token_keyword_kind_to_str(kind));
        struct report_info info = {REPORT_ERROR, tstream_last(ts)->span, what,
                                   ""};
        report(ctx, &info);
        return false;
    } else {
        return true;
    }
}

bool tstream_check_int(struct context *ctx, struct tstream *ts) {
    if (!tstream_is_int(ts)) {
        struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                   "expected integer after this", ""};
        report(ctx, &info);
        return false;
    } else {
        return true;
    }
}

bool tstream_check_float(struct context *ctx, struct tstream *ts) {
    if (!tstream_is_float(ts)) {
        struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                   "expected floating number after this", ""};
        report(ctx, &info);
        return false;
    } else {
        return true;
    }
}

bool tstream_check_ident(struct context *ctx, struct tstream *ts) {
    if (!tstream_is_ident(ts)) {
        struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                   "expected identifier after this", ""};
        report(ctx, &info);
        return false;
    } else {
        return true;
    }
}

bool tstream_check_string(struct context *ctx, struct tstream *ts) {
    if (!tstream_is_string(ts)) {
        struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                   "expected string after this", ""};
        report(ctx, &info);
        return false;
    } else {
        return true;
    }
}

bool tstream_check_char(struct context *ctx, struct tstream *ts) {
    if (!tstream_is_char(ts)) {
        struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                   "expected character after this", ""};
        report(ctx, &info);
        return false;
    } else {
        return true;
    }
}

bool tstream_is_punct(struct tstream *ts, enum token_punct_kind kind) {
    return !tstream_eos(ts) && tstream_curr(ts)->kind == TK_PUNCT &&
           tstream_curr(ts)->punct.kind == kind;
}

bool tstream_is_keyword(struct tstream *ts, enum token_keyword_kind kind) {
    return !tstream_eos(ts) && tstream_curr(ts)->kind == TK_KEYWORD &&
           tstream_curr(ts)->keyword.kind == kind;
}

bool tstream_is_int(struct tstream *ts) {
    return !tstream_eos(ts) && tstream_curr(ts)->kind == TK_INT;
}

bool tstream_is_float(struct tstream *ts) {
    return !tstream_eos(ts) && tstream_curr(ts)->kind == TK_FLOAT;
}

bool tstream_is_ident(struct tstream *ts) {
    return !tstream_eos(ts) && tstream_curr(ts)->kind == TK_IDENT;
}

bool tstream_is_string(struct tstream *ts) {
    return !tstream_eos(ts) && tstream_curr(ts)->kind == TK_STRING;
}

bool tstream_is_char(struct tstream *ts) {
    return !tstream_eos(ts) && tstream_curr(ts)->kind == TK_CHAR;
}
