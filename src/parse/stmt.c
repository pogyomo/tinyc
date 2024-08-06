#include "stmt.h"

#include "ast/expr.h"
#include "decl.h"
#include "expr.h"
#include "stream.h"

bool parse_stmt(struct parse_context *ctx, struct tstream *ts,
                struct stmt **stmt) {
    if (tstream_is_ident(ts)) {
        struct tstream ts_save = *ts;
        tstream_advance(ts);
        if (tstream_is_punct(ts, TK_PUNCT_COLON)) {
            *ts = ts_save;
            return parse_label_stmt(ctx, ts, stmt);
        } else {
            *ts = ts_save;
            return parse_expr_stmt(ctx, ts, stmt);
        }
    } else if (tstream_is_keyword(ts, TK_KEYWORD_CASE)) {
        return parse_case_stmt(ctx, ts, stmt);
    } else if (tstream_is_keyword(ts, TK_KEYWORD_DEFAULT)) {
        return parse_default_stmt(ctx, ts, stmt);
    } else if (tstream_is_punct(ts, TK_PUNCT_LCURLY)) {
        return parse_block_stmt(ctx, ts, stmt);
    } else if (tstream_is_keyword(ts, TK_KEYWORD_IF)) {
        return parse_if_stmt(ctx, ts, stmt);
    } else if (tstream_is_keyword(ts, TK_KEYWORD_SWITCH)) {
        return parse_switch_stmt(ctx, ts, stmt);
    } else if (tstream_is_keyword(ts, TK_KEYWORD_WHILE)) {
        return parse_while_stmt(ctx, ts, stmt);
    } else if (tstream_is_keyword(ts, TK_KEYWORD_DO)) {
        return parse_do_while_stmt(ctx, ts, stmt);
    } else if (tstream_is_keyword(ts, TK_KEYWORD_FOR)) {
        return parse_for_stmt(ctx, ts, stmt);
    } else if (tstream_is_keyword(ts, TK_KEYWORD_GOTO)) {
        return parse_goto_stmt(ctx, ts, stmt);
    } else if (tstream_is_keyword(ts, TK_KEYWORD_CONTINUE)) {
        return parse_continue_stmt(ctx, ts, stmt);
    } else if (tstream_is_keyword(ts, TK_KEYWORD_BREAK)) {
        return parse_break_stmt(ctx, ts, stmt);
    } else if (tstream_is_keyword(ts, TK_KEYWORD_RETURN)) {
        return parse_return_stmt(ctx, ts, stmt);
    } else {
        return parse_expr_stmt(ctx, ts, stmt);
    }
}

bool parse_label_stmt(struct parse_context *ctx, struct tstream *ts,
                      struct stmt **stmt) {
    struct span span;

    struct stmt_label_label label;
    TRY(tstream_check_ident(ctx->ctx, ts));
    label.value = tstream_curr(ts)->ident.value;
    label.span = tstream_curr(ts)->span;
    span = label.span;
    tstream_advance(ts);

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_COLON));
    tstream_advance(ts);

    struct stmt *stmt_;
    TRY(parse_stmt(ctx, ts, &stmt_));
    merge_span(&span, &stmt_->span, &span);

    *stmt = stmt_label_new(&label, stmt_, &span);
    return true;
}

bool parse_case_stmt(struct parse_context *ctx, struct tstream *ts,
                     struct stmt **stmt) {
    struct span span;

    TRY(tstream_check_keyword(ctx->ctx, ts, TK_KEYWORD_CASE));
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    struct expr *expr;
    TRY(parse_expr(ctx, ts, &expr));

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_COLON));
    tstream_advance(ts);

    struct stmt *stmt_;
    TRY(parse_stmt(ctx, ts, &stmt_));
    merge_span(&span, &stmt_->span, &span);

    *stmt = stmt_case_new(expr, stmt_, &span);
    return true;
}

bool parse_default_stmt(struct parse_context *ctx, struct tstream *ts,
                        struct stmt **stmt) {
    struct span span;

    TRY(tstream_check_keyword(ctx->ctx, ts, TK_KEYWORD_DEFAULT));
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_COLON));
    tstream_advance(ts);

    struct stmt *stmt_;
    TRY(parse_stmt(ctx, ts, &stmt_));
    merge_span(&span, &stmt_->span, &span);

    *stmt = stmt_default_new(stmt_, &span);
    return true;
}

bool parse_block_stmt(struct parse_context *ctx, struct tstream *ts,
                      struct stmt **stmt) {
    struct span span;

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_LCURLY));
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    struct stmt_block_item item_head = {NULL};
    struct stmt_block_item *item_prev = &item_head;
    while (true) {
        TRY(tstream_check_eos(ctx->ctx, ts));
        if (tstream_is_punct(ts, TK_PUNCT_RCURLY)) {
            break;
        } else {
            struct decl *decl;
            struct stmt *stmt;
            context_suppress_report(ctx->ctx);
            if (parse_decl(ctx, ts, &decl)) {
                context_activate_report(ctx->ctx);
                item_prev->next = stmt_block_item_decl_new(decl);
                item_prev = item_prev->next;
            } else {
                context_activate_report(ctx->ctx);
                TRY(parse_stmt(ctx, ts, &stmt));
                item_prev->next = stmt_block_item_stmt_new(stmt);
                item_prev = item_prev->next;
            }
        }
    }
    merge_span(&span, &tstream_curr(ts)->span, &span);
    tstream_advance(ts);

    *stmt = stmt_block_new(item_head.next, &span);
    return true;
}

bool parse_expr_stmt(struct parse_context *ctx, struct tstream *ts,
                     struct stmt **stmt) {
    struct span span;

    struct expr *expr = NULL;
    if (!tstream_is_punct(ts, TK_PUNCT_SEMICOLON)) {
        TRY(parse_expr(ctx, ts, &expr));
        TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_SEMICOLON));
        merge_span(&expr->span, &tstream_curr(ts)->span, &span);
    } else {
        span = tstream_curr(ts)->span;
    }
    tstream_advance(ts);

    *stmt = stmt_expr_new(expr, &span);
    return true;
}

bool parse_if_stmt(struct parse_context *ctx, struct tstream *ts,
                   struct stmt **stmt) {
    struct span span;

    TRY(tstream_check_keyword(ctx->ctx, ts, TK_KEYWORD_IF));
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_LPAREN));
    tstream_advance(ts);

    struct expr *cond;
    TRY(parse_expr(ctx, ts, &cond));

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_RPAREN));
    tstream_advance(ts);

    struct stmt *then;
    TRY(parse_stmt(ctx, ts, &then));
    merge_span(&span, &then->span, &span);

    struct stmt *else_ = NULL;
    if (tstream_is_keyword(ts, TK_KEYWORD_ELSE)) {
        tstream_advance(ts);
        TRY(parse_stmt(ctx, ts, &else_));
        merge_span(&span, &else_->span, &span);
    }

    *stmt = stmt_if_new(cond, then, else_, &span);
    return true;
}

bool parse_switch_stmt(struct parse_context *ctx, struct tstream *ts,
                       struct stmt **stmt) {
    struct span span;

    TRY(tstream_check_keyword(ctx->ctx, ts, TK_KEYWORD_SWITCH));
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_LPAREN));
    tstream_advance(ts);

    struct expr *expr;
    TRY(parse_expr(ctx, ts, &expr));

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_RPAREN));
    tstream_advance(ts);

    struct stmt *body;
    TRY(parse_stmt(ctx, ts, &body));
    merge_span(&span, &body->span, &span);

    *stmt = stmt_switch_new(expr, body, &span);
    return true;
}

bool parse_while_stmt(struct parse_context *ctx, struct tstream *ts,
                      struct stmt **stmt) {
    struct span span;

    TRY(tstream_check_keyword(ctx->ctx, ts, TK_KEYWORD_WHILE));
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_LPAREN));
    tstream_advance(ts);

    struct expr *cond;
    TRY(parse_expr(ctx, ts, &cond));

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_RPAREN));
    tstream_advance(ts);

    struct stmt *body;
    TRY(parse_stmt(ctx, ts, &body));
    merge_span(&span, &body->span, &span);

    *stmt = stmt_do_while_new(cond, body, &span);
    return true;
}

bool parse_do_while_stmt(struct parse_context *ctx, struct tstream *ts,
                         struct stmt **stmt) {
    struct span span;

    TRY(tstream_check_keyword(ctx->ctx, ts, TK_KEYWORD_DO));
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    struct stmt *body;
    TRY(parse_stmt(ctx, ts, &body));

    TRY(tstream_check_keyword(ctx->ctx, ts, TK_KEYWORD_WHILE));
    tstream_advance(ts);

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_LPAREN));
    tstream_advance(ts);

    struct expr *cond;
    TRY(parse_expr(ctx, ts, &cond));

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_RPAREN));
    tstream_advance(ts);

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_SEMICOLON));
    merge_span(&span, &tstream_curr(ts)->span, &span);
    tstream_advance(ts);

    *stmt = stmt_do_while_new(cond, body, &span);
    return true;
}

bool parse_for_stmt(struct parse_context *ctx, struct tstream *ts,
                    struct stmt **stmt) {
    struct span span;

    TRY(tstream_check_keyword(ctx->ctx, ts, TK_KEYWORD_FOR));
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_LPAREN));
    tstream_advance(ts);

    struct stmt_for_init *init = NULL;
    if (!tstream_is_punct(ts, TK_PUNCT_SEMICOLON)) {
        struct decl *decl;
        struct expr *expr;
        context_suppress_report(ctx->ctx);
        if (parse_decl(ctx, ts, &decl)) {
            context_activate_report(ctx->ctx);
            init = stmt_for_init_decl_new(decl);
        } else {
            context_activate_report(ctx->ctx);
            TRY(parse_expr(ctx, ts, &expr));
            init = stmt_for_init_expr_new(expr);

            TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_SEMICOLON));
            tstream_advance(ts);
        }
    }
    tstream_advance(ts);

    struct expr *cond = NULL;
    if (!tstream_is_punct(ts, TK_PUNCT_SEMICOLON)) {
        TRY(parse_expr(ctx, ts, &cond));
        TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_SEMICOLON));
    }
    tstream_advance(ts);

    struct expr *update = NULL;
    if (!tstream_is_punct(ts, TK_PUNCT_RPAREN)) {
        TRY(parse_expr(ctx, ts, &update));
        TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_RPAREN));
    }
    tstream_advance(ts);

    struct stmt *body;
    TRY(parse_stmt(ctx, ts, &body));
    merge_span(&span, &body->span, &span);

    *stmt = stmt_for_new(init, cond, update, body, &span);
    return true;
}

bool parse_goto_stmt(struct parse_context *ctx, struct tstream *ts,
                     struct stmt **stmt) {
    struct span span;

    TRY(tstream_check_keyword(ctx->ctx, ts, TK_KEYWORD_GOTO));
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    struct stmt_goto_label label;
    TRY(tstream_check_ident(ctx->ctx, ts));
    label.value = tstream_curr(ts)->ident.value;
    label.span = tstream_curr(ts)->span;
    tstream_advance(ts);

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_SEMICOLON));
    merge_span(&span, &tstream_curr(ts)->span, &span);
    tstream_advance(ts);

    *stmt = stmt_goto_new(&label, &span);
    return true;
}

bool parse_continue_stmt(struct parse_context *ctx, struct tstream *ts,
                         struct stmt **stmt) {
    struct span span;

    TRY(tstream_check_keyword(ctx->ctx, ts, TK_KEYWORD_CONTINUE));
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_SEMICOLON));
    merge_span(&span, &tstream_curr(ts)->span, &span);
    tstream_advance(ts);

    *stmt = stmt_continue_new(&span);
    return true;
}

bool parse_break_stmt(struct parse_context *ctx, struct tstream *ts,
                      struct stmt **stmt) {
    struct span span;

    TRY(tstream_check_keyword(ctx->ctx, ts, TK_KEYWORD_BREAK));
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_SEMICOLON));
    merge_span(&span, &tstream_curr(ts)->span, &span);
    tstream_advance(ts);

    *stmt = stmt_break_new(&span);
    return true;
}

bool parse_return_stmt(struct parse_context *ctx, struct tstream *ts,
                       struct stmt **stmt) {
    struct span span;

    TRY(tstream_check_keyword(ctx->ctx, ts, TK_KEYWORD_RETURN));
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    struct expr *expr;
    TRY(parse_expr(ctx, ts, &expr));

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_SEMICOLON));
    merge_span(&span, &tstream_curr(ts)->span, &span);
    tstream_advance(ts);

    *stmt = stmt_return_new(expr, &span);
    return true;
}
