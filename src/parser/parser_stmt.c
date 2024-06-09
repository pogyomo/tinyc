#include "parser_stmt.h"

#include "parser_decl.h"
#include "parser_expr.h"
#include "utils.h"

bool parse_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    TRY(check_eos(ctx, ts));
    if (tstream_is(ts, TK_IDENTIFIER)) {
        if (parse_labeled_stmt(ctx, ts, stmt)) {
            return true;
        } else {
            return parse_expr_stmt(ctx, ts, stmt);
        }
    } else if (tstream_is(ts, TK_CASE)) {
        return parse_case_stmt(ctx, ts, stmt);
    } else if (tstream_is(ts, TK_DEFAULT)) {
        return parse_default_stmt(ctx, ts, stmt);
    } else if (tstream_is(ts, TK_LCURLY)) {
        return parse_block_stmt(ctx, ts, stmt);
    } else if (tstream_is(ts, TK_IF)) {
        return parse_if_stmt(ctx, ts, stmt);
    } else if (tstream_is(ts, TK_SWITCH)) {
        return parse_switch_stmt(ctx, ts, stmt);
    } else if (tstream_is(ts, TK_WHILE)) {
        return parse_while_stmt(ctx, ts, stmt);
    } else if (tstream_is(ts, TK_DO)) {
        return parse_do_while_stmt(ctx, ts, stmt);
    } else if (tstream_is(ts, TK_GOTO)) {
        return parse_goto_stmt(ctx, ts, stmt);
    } else if (tstream_is(ts, TK_BREAK)) {
        return parse_break_stmt(ctx, ts, stmt);
    } else if (tstream_is(ts, TK_RETURN)) {
        return parse_return_stmt(ctx, ts, stmt);
    } else {
        return parse_expr_stmt(ctx, ts, stmt);
    }
}

bool parse_labeled_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    TRY(check_kind(ctx, ts, TK_IDENTIFIER));
    token_t *label = tstream_token(ts);
    tstream_advance(ts);

    TRY(expect_kind(ctx, ts, TK_COLON, NULL));

    stmt_t *after = stmt_alloc();
    TRY(parse_stmt(ctx, ts, after));

    stmt->kind = STMT_LABELED;
    stmt->labeled.stmt = after;
    stmt->labeled.label.name = label->repr;
    stmt->labeled.label.span = label->span;
    stmt->span = concat_span(label->span, after->span);
    return true;
}

bool parse_case_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    span_t first;
    TRY(expect_kind(ctx, ts, TK_CASE, &first));

    expr_t *expr = expr_alloc();
    TRY(parse_const_expr(ctx, ts, expr));

    TRY(expect_kind(ctx, ts, TK_COLON, NULL));

    stmt_t *after = stmt_alloc();
    TRY(parse_stmt(ctx, ts, after));

    stmt->kind = STMT_CASE;
    stmt->case_.expr = expr;
    stmt->case_.stmt = after;
    stmt->span = concat_span(first, after->span);
    return true;
}

bool parse_default_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    span_t first;
    TRY(expect_kind(ctx, ts, TK_DEFAULT, &first));

    TRY(expect_kind(ctx, ts, TK_COLON, NULL));

    stmt_t *after = stmt_alloc();
    TRY(parse_stmt(ctx, ts, after));

    stmt->kind = STMT_CASE;
    stmt->default_.stmt = after;
    stmt->span = concat_span(first, after->span);
    return true;
}

bool parse_expr_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    TRY(check_eos(ctx, ts));

    token_t *token = tstream_token(ts);
    if (token->kind == TK_SEMICOLON) {
        tstream_advance(ts);
        stmt->kind = STMT_EXPR;
        stmt->expr.expr = NULL;
        stmt->span = token->span;
        return true;
    } else {
        expr_t *expr = expr_alloc();
        TRY(parse_expr(ctx, ts, expr));

        span_t last;
        TRY(expect_kind(ctx, ts, TK_SEMICOLON, &last));

        stmt->kind = STMT_EXPR;
        stmt->expr.expr = expr;
        stmt->span = concat_span(expr->span, last);
        return true;
    }
}

bool parse_block_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    span_t first, last;
    TRY(expect_kind(ctx, ts, TK_LCURLY, &first));

    vector_t items;
    vector_init(&items, sizeof(stmt_block_item_t));
    stmt_t *item_s;
    decl_t *item_d;
    tstream_state_t state;
    while (true) {
        TRY(check_eos(ctx, ts));
        if (tstream_expect(ts, TK_RCURLY, &last)) {
            break;
        }

        state = tstream_state(ts);
        if (!parse_decl(ctx, ts, item_d)) {
            tstream_set_state(ts, state);
            TRY(parse_stmt(ctx, ts, item_s));
            stmt_block_item_t item;
            item.kind = STMT_BLOCK_ITEM_STMT;
            item.stmt = item_s;
            vector_push(&items, &item);
        } else {
            stmt_block_item_t item;
            item.kind = STMT_BLOCK_ITEM_DECL;
            item.decl = item_d;
            vector_push(&items, &item);
        }
    }

    stmt->kind = STMT_BLOCK;
    stmt->block.items = items;
    stmt->span = concat_span(first, last);
    return true;
}

bool parse_if_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    span_t first;
    TRY(expect_kind(ctx, ts, TK_IF, &first));
    TRY(expect_kind(ctx, ts, TK_LPAREN, NULL));

    expr_t *cond = expr_alloc();
    TRY(parse_expr(ctx, ts, cond));

    TRY(expect_kind(ctx, ts, TK_RPAREN, NULL));

    stmt_t *then = stmt_alloc();
    TRY(parse_stmt(ctx, ts, then));

    if (tstream_is(ts, TK_ELSE)) {
        tstream_advance(ts);
        stmt_t *else_ = stmt_alloc();
        TRY(parse_stmt(ctx, ts, else_));

        stmt->kind = STMT_IF;
        stmt->if_.cond = cond;
        stmt->if_.then = then;
        stmt->if_.else_ = else_;
        stmt->span = concat_span(first, else_->span);
    } else {
        stmt->kind = STMT_IF;
        stmt->if_.cond = cond;
        stmt->if_.then = then;
        stmt->if_.else_ = NULL;
        stmt->span = concat_span(first, then->span);
    }
    return true;
}

bool parse_switch_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    span_t first;
    TRY(expect_kind(ctx, ts, TK_SWITCH, &first));
    TRY(expect_kind(ctx, ts, TK_LPAREN, NULL));

    expr_t *expr = expr_alloc();
    TRY(parse_expr(ctx, ts, expr));

    TRY(expect_kind(ctx, ts, TK_RPAREN, NULL));

    stmt_t *body = stmt_alloc();
    TRY(parse_stmt(ctx, ts, body));

    stmt->kind = STMT_SWITCH;
    stmt->switch_.expr = expr;
    stmt->switch_.body = body;
    stmt->span = concat_span(first, body->span);
    return true;
}

bool parse_while_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    span_t first;
    TRY(expect_kind(ctx, ts, TK_WHILE, &first));
    TRY(expect_kind(ctx, ts, TK_LPAREN, NULL));

    expr_t *cond = expr_alloc();
    TRY(parse_expr(ctx, ts, cond));

    stmt_t *body = stmt_alloc();
    TRY(parse_stmt(ctx, ts, body));

    stmt->kind = STMT_WHILE;
    stmt->while_.cond = cond;
    stmt->while_.body = body;
    stmt->span = concat_span(first, body->span);
    return true;
}

bool parse_do_while_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    span_t first, last;
    TRY(expect_kind(ctx, ts, TK_DO, &first));

    stmt_t *body = stmt_alloc();
    TRY(parse_stmt(ctx, ts, body));

    TRY(expect_kind(ctx, ts, TK_WHILE, NULL));
    TRY(expect_kind(ctx, ts, TK_LPAREN, NULL));

    expr_t *cond = expr_alloc();
    TRY(parse_expr(ctx, ts, cond));

    TRY(expect_kind(ctx, ts, TK_RPAREN, NULL));
    TRY(expect_kind(ctx, ts, TK_SEMICOLON, &last));

    stmt->kind = STMT_WHILE;
    stmt->while_.cond = cond;
    stmt->while_.body = body;
    stmt->span = concat_span(first, last);
    return true;
}

bool parse_for_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    span_t first;
    TRY(expect_kind(ctx, ts, TK_FOR, &first));
    TRY(expect_kind(ctx, ts, TK_LPAREN, NULL));

    decl_t *init_d = NULL;
    expr_t *init_e = NULL;
    tstream_state_t state = tstream_state(ts);
    init_d = decl_alloc();
    if (!parse_decl(ctx, ts, init_d)) {
        init_d = NULL;
        init_e = expr_alloc();
        tstream_set_state(ts, state);
        TRY(parse_expr(ctx, ts, init_e));
        TRY(expect_kind(ctx, ts, TK_SEMICOLON, NULL));
    }

    expr_t *cond = expr_alloc();
    TRY(parse_expr(ctx, ts, cond));
    TRY(expect_kind(ctx, ts, TK_SEMICOLON, NULL));

    expr_t *update = expr_alloc();
    TRY(parse_expr(ctx, ts, update));

    stmt_t *body = stmt_alloc();
    TRY(parse_stmt(ctx, ts, body));

    stmt->kind = STMT_FOR;
    if (init_d != NULL) {
        stmt->for_.init.kind = STMT_IF_INIT_DECL;
        stmt->for_.init.decl = init_d;
    } else {
        stmt->for_.init.kind = STMT_IF_INIT_EXPR;
        stmt->for_.init.expr = init_e;
    }
    stmt->span = concat_span(first, body->span);
    return true;
}

bool parse_goto_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    span_t first, last;
    TRY(expect_kind(ctx, ts, TK_GOTO, &first));

    TRY(check_kind(ctx, ts, TK_IDENTIFIER));
    token_t *ident = tstream_token(ts);
    tstream_advance(ts);

    TRY(expect_kind(ctx, ts, TK_SEMICOLON, &last));

    stmt->kind = STMT_GOTO;
    stmt->goto_.label.name = ident->repr;
    stmt->goto_.label.span = ident->span;
    stmt->span = concat_span(first, last);
    return true;
}

bool parse_continue_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    span_t first, last;
    TRY(expect_kind(ctx, ts, TK_CONTINUE, &first));
    TRY(expect_kind(ctx, ts, TK_SEMICOLON, &last));

    stmt->kind = STMT_CONTINUE;
    stmt->span = concat_span(first, last);
    return true;
}

bool parse_break_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    span_t first, last;
    TRY(expect_kind(ctx, ts, TK_BREAK, &first));
    TRY(expect_kind(ctx, ts, TK_SEMICOLON, &last));

    stmt->kind = STMT_BREAK;
    stmt->span = concat_span(first, last);
    return true;
}

bool parse_return_stmt(context_t *ctx, tstream_t *ts, stmt_t *stmt) {
    span_t first, last;
    TRY(expect_kind(ctx, ts, TK_RETURN, &first));

    expr_t *expr = NULL;
    TRY(check_eos(ctx, ts));
    if (!tstream_expect(ts, TK_SEMICOLON, &last)) {
        expr = expr_alloc();
        TRY(parse_expr(ctx, ts, expr));
        TRY(expect_kind(ctx, ts, TK_SEMICOLON, &last));
    }

    stmt->kind = STMT_RETURN;
    stmt->return_.expr = expr;
    stmt->span = concat_span(first, last);
    return true;
}
