#include "parser_expr.h"

#include <stdlib.h>

#include "../report.h"
#include "utils.h"

bool parse_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    expr_t *lhs, *rhs, *tmp;
    span_t op_span;

    lhs = expr_alloc();
    TRY(parse_assign_expr(ctx, ts, lhs));
    while (!tstream_eos(ts)) {
        if (tstream_expect(ts, TK_COMMA, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_COMMA;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else {
            break;
        }
    }
    memcpy(expr, lhs, sizeof(expr_t));
    return true;
}

bool parse_assign_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    tstream_state_t state = tstream_state(ts);
    expr_t *lhs = expr_alloc();
    if (!parse_unary_expr(ctx, ts, lhs)) {
        tstream_set_state(ts, state);
        return parse_cond_expr(ctx, ts, expr);
    }

    TRY(check_kinds(ctx, ts, 11, TK_ASSIGN, TK_ORASSIGN, TK_XORASSIGN,
                    TK_ANDASSIGN, TK_LSHIFTASSIGN, TK_RSHIFTASSIGN,
                    TK_ADDASSIGN, TK_SUBASSIGN, TK_MULASSIGN, TK_DIVASSIGN,
                    TK_MODASSIGN));
    token_t *op = tstream_token(ts);
    tstream_advance(ts);

    expr_t *rhs = expr_alloc();
    TRY(parse_unary_expr(ctx, ts, rhs));

    expr->kind = EXPR_INFIX;
    expr->infix.op.span = op->span;
    if (op->kind == TK_ASSIGN) {
        expr->infix.op.kind = EXPR_INFIX_ASSIGN;
    } else if (op->kind == TK_ORASSIGN) {
        expr->infix.op.kind = EXPR_INFIX_ORASSIGN;
    } else if (op->kind == TK_XORASSIGN) {
        expr->infix.op.kind = EXPR_INFIX_XORASSIGN;
    } else if (op->kind == TK_ANDASSIGN) {
        expr->infix.op.kind = EXPR_INFIX_ANDASSIGN;
    } else if (op->kind == TK_LSHIFTASSIGN) {
        expr->infix.op.kind = EXPR_INFIX_LSHIFTASSIGN;
    } else if (op->kind == TK_RSHIFTASSIGN) {
        expr->infix.op.kind = EXPR_INFIX_RSHIFTASSIGN;
    } else if (op->kind == TK_ADDASSIGN) {
        expr->infix.op.kind = EXPR_INFIX_ADDASSIGN;
    } else if (op->kind == TK_SUBASSIGN) {
        expr->infix.op.kind = EXPR_INFIX_SUBASSIGN;
    } else if (op->kind == TK_MULASSIGN) {
        expr->infix.op.kind = EXPR_INFIX_MULASSIGN;
    } else if (op->kind == TK_DIVASSIGN) {
        expr->infix.op.kind = EXPR_INFIX_DIVASSIGN;
    } else {
        expr->infix.op.kind = EXPR_INFIX_MODASSIGN;
    }
    expr->infix.lhs = lhs;
    expr->infix.rhs = rhs;
    expr->span = concat_span(lhs->span, rhs->span);
    return true;
}

bool parse_const_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    return parse_cond_expr(ctx, ts, expr);
}

bool parse_cond_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    expr_t *cond = expr_alloc();
    TRY(parse_logical_or_expr(ctx, ts, cond));
    if (!tstream_is(ts, TK_QUESTION)) {
        memcpy(expr, cond, sizeof(expr_t));
        return true;
    }

    expr_t *then = expr_alloc();
    TRY(parse_expr(ctx, ts, then));
    TRY(expect_kind(ctx, ts, TK_COLON, NULL));

    expr_t *else_ = expr_alloc();
    TRY(parse_expr(ctx, ts, else_));

    expr->kind = EXPR_COND;
    expr->cond.cond = cond;
    expr->cond.then = then;
    expr->cond.else_ = else_;
    expr->span = concat_span(cond->span, else_->span);
    return true;
}

bool parse_logical_or_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    expr_t *lhs, *rhs, *tmp;
    span_t op_span;

    lhs = expr_alloc();
    TRY(parse_assign_expr(ctx, ts, lhs));
    while (!tstream_eos(ts)) {
        if (tstream_expect(ts, TK_OR, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_OR;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else {
            break;
        }
    }
    memcpy(expr, lhs, sizeof(expr_t));
    return true;
}

bool parse_logical_and_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    expr_t *lhs, *rhs, *tmp;
    span_t op_span;

    lhs = expr_alloc();
    TRY(parse_assign_expr(ctx, ts, lhs));
    while (!tstream_eos(ts)) {
        if (tstream_expect(ts, TK_AND, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_AND;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else {
            break;
        }
    }
    memcpy(expr, lhs, sizeof(expr_t));
    return true;
}

bool parse_or_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    expr_t *lhs, *rhs, *tmp;
    span_t op_span;

    lhs = expr_alloc();
    TRY(parse_assign_expr(ctx, ts, lhs));
    while (!tstream_eos(ts)) {
        if (tstream_expect(ts, TK_VERTICAL, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_BITOR;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else {
            break;
        }
    }
    memcpy(expr, lhs, sizeof(expr_t));
    return true;
}

bool parse_xor_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    expr_t *lhs, *rhs, *tmp;
    span_t op_span;

    lhs = expr_alloc();
    TRY(parse_assign_expr(ctx, ts, lhs));
    while (!tstream_eos(ts)) {
        if (tstream_expect(ts, TK_HAT, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_BITXOR;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else {
            break;
        }
    }
    memcpy(expr, lhs, sizeof(expr_t));
    return true;
}

bool parse_and_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    expr_t *lhs, *rhs, *tmp;
    span_t op_span;

    lhs = expr_alloc();
    TRY(parse_assign_expr(ctx, ts, lhs));
    while (!tstream_eos(ts)) {
        if (tstream_expect(ts, TK_AMPERSAND, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_BITAND;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else {
            break;
        }
    }
    memcpy(expr, lhs, sizeof(expr_t));
    return true;
}

bool parse_equality_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    expr_t *lhs, *rhs, *tmp;
    span_t op_span;

    lhs = expr_alloc();
    TRY(parse_assign_expr(ctx, ts, lhs));
    while (!tstream_eos(ts)) {
        if (tstream_expect(ts, TK_EQ, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_EQ;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else if (tstream_expect(ts, TK_NE, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_NE;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else {
            break;
        }
    }
    memcpy(expr, lhs, sizeof(expr_t));
    return true;
}

bool parse_relational_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    expr_t *lhs, *rhs, *tmp;
    span_t op_span;

    lhs = expr_alloc();
    TRY(parse_assign_expr(ctx, ts, lhs));
    while (!tstream_eos(ts)) {
        if (tstream_expect(ts, TK_LT, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_LT;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else if (tstream_expect(ts, TK_GT, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_GT;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else if (tstream_expect(ts, TK_LE, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_LE;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else if (tstream_expect(ts, TK_GE, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_GE;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else {
            break;
        }
    }
    memcpy(expr, lhs, sizeof(expr_t));
    return true;
}

bool parse_shift_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    expr_t *lhs, *rhs, *tmp;
    span_t op_span;

    lhs = expr_alloc();
    TRY(parse_assign_expr(ctx, ts, lhs));
    while (!tstream_eos(ts)) {
        if (tstream_expect(ts, TK_LSHIFT, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_LSHIFT;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else if (tstream_expect(ts, TK_RSHIFT, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_RSHIFT;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else {
            break;
        }
    }
    memcpy(expr, lhs, sizeof(expr_t));
    return true;
}

bool parse_additive_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    expr_t *lhs, *rhs, *tmp;
    span_t op_span;

    lhs = expr_alloc();
    TRY(parse_assign_expr(ctx, ts, lhs));
    while (!tstream_eos(ts)) {
        if (tstream_expect(ts, TK_PLUS, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_ADD;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else if (tstream_expect(ts, TK_MINUS, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_SUB;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else {
            break;
        }
    }
    memcpy(expr, lhs, sizeof(expr_t));
    return true;
}

bool parse_multiplicative_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    expr_t *lhs, *rhs, *tmp;
    span_t op_span;

    lhs = expr_alloc();
    TRY(parse_assign_expr(ctx, ts, lhs));
    while (!tstream_eos(ts)) {
        if (tstream_expect(ts, TK_STAR, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_MUL;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else if (tstream_expect(ts, TK_SLASH, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_DIV;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else if (tstream_expect(ts, TK_PERCENT, &op_span)) {
            rhs = expr_alloc();
            TRY(parse_assign_expr(ctx, ts, rhs));

            tmp = expr_alloc();
            tmp->kind = EXPR_INFIX;
            tmp->infix.op.kind = EXPR_INFIX_MOD;
            tmp->infix.op.span = op_span;
            tmp->infix.lhs = lhs;
            tmp->infix.rhs = rhs;
            tmp->span = concat_span(lhs->span, rhs->span);
            lhs = tmp;
        } else {
            break;
        }
    }
    memcpy(expr, lhs, sizeof(expr_t));
    return true;
}

bool parse_cast_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    // TODO: Parse cast expression
    return parse_unary_expr(ctx, ts, expr);
}

bool parse_unary_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    token_t *token;
    expr_t *e = expr_alloc();

    TRY(check_eos(ctx, ts));
    token = tstream_token(ts);
    tstream_advance(ts);
    if (token->kind == TK_PLUSPLUS) {
        TRY(parse_unary_expr(ctx, ts, e));
        expr->kind = EXPR_UNARY;
        expr->unary.op.span = token->span;
        expr->unary.op.kind = EXPR_UNARY_INC;
        expr->unary.expr = e;
        return true;
    } else if (token->kind == TK_MINUSMINUS) {
        TRY(parse_unary_expr(ctx, ts, e));
        expr->kind = EXPR_UNARY;
        expr->unary.op.span = token->span;
        expr->unary.op.kind = EXPR_UNARY_DEC;
        expr->unary.expr = e;
        return true;
    } else if (token->kind == TK_AMPERSAND) {
        TRY(parse_cast_expr(ctx, ts, e));
        expr->kind = EXPR_UNARY;
        expr->unary.op.span = token->span;
        expr->unary.op.kind = EXPR_UNARY_REF;
        expr->unary.expr = e;
        return true;
    } else if (token->kind == TK_STAR) {
        TRY(parse_cast_expr(ctx, ts, e));
        expr->kind = EXPR_UNARY;
        expr->unary.op.span = token->span;
        expr->unary.op.kind = EXPR_UNARY_DEREF;
        expr->unary.expr = e;
        return true;
    } else if (token->kind == TK_PLUS) {
        TRY(parse_cast_expr(ctx, ts, e));
        expr->kind = EXPR_UNARY;
        expr->unary.op.span = token->span;
        expr->unary.op.kind = EXPR_UNARY_PLUS;
        expr->unary.expr = e;
        return true;
    } else if (token->kind == TK_MINUS) {
        TRY(parse_cast_expr(ctx, ts, e));
        expr->kind = EXPR_UNARY;
        expr->unary.op.span = token->span;
        expr->unary.op.kind = EXPR_UNARY_MINUS;
        expr->unary.expr = e;
        return true;
    } else if (token->kind == TK_TILDE) {
        TRY(parse_cast_expr(ctx, ts, e));
        expr->kind = EXPR_UNARY;
        expr->unary.op.span = token->span;
        expr->unary.op.kind = EXPR_UNARY_INV;
        expr->unary.expr = e;
        return true;
    } else if (token->kind == TK_NOT) {
        TRY(parse_cast_expr(ctx, ts, e));
        expr->kind = EXPR_UNARY;
        expr->unary.op.span = token->span;
        expr->unary.op.kind = EXPR_UNARY_NEG;
        expr->unary.expr = e;
        return true;
    } else if (token->kind == TK_SIZEOF) {
        // TODO: Parse sizeof
        string_t what, info;
        string_from(&what, "sizeof is not supported");
        string_init(&info);
        report(ctx, REPORT_LEVEL_ERROR,
               (report_info_t){what, info, token->span});
        return false;
    } else {
        return parse_postfix_expr(ctx, ts, expr);
    }
}

bool parse_postfix_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    expr_t *e, *tmp;
    span_t rspan, op_span;

    e = expr_alloc();
    TRY(parse_primary_expr(ctx, ts, e));
    while (!tstream_eos(ts)) {
        if (tstream_expect(ts, TK_LSQUARE, &op_span)) {
            expr_t *index = expr_alloc();
            TRY(parse_expr(ctx, ts, index));
            TRY(expect_kind(ctx, ts, TK_RSQUARE, &rspan));
            tmp = expr_alloc();
            tmp->kind = EXPR_INDEX;
            tmp->index.index = index;
            tmp->index.expr = e;
            tmp->span = concat_span(e->span, rspan);
            e = tmp;
        } else if (tstream_expect(ts, TK_LPAREN, &op_span)) {
            span_t rspan;
            vector_t args;
            vector_init(&args, sizeof(expr_t *));
            while (true) {
                TRY(check_eos(ctx, ts));
                if (tstream_expect(ts, TK_RPAREN, &rspan)) {
                    break;
                }

                expr_t *arg = expr_alloc();
                parse_assign_expr(ctx, ts, arg);
                vector_push(&args, &arg);

                TRY(check_kinds(ctx, ts, 2, TK_RPAREN, TK_COMMA));
                if (tstream_expect(ts, TK_RPAREN, &rspan)) {
                    break;
                } else {
                    tstream_advance(ts);
                    continue;
                }
            }
            tmp = expr_alloc();
            tmp->kind = EXPR_CALL;
            tmp->call.expr = e;
            tmp->call.args = args;
            tmp->span = concat_span(e->span, rspan);
            e = tmp;
        } else if (tstream_expect(ts, TK_DOT, &op_span)) {
            TRY(check_kind(ctx, ts, TK_IDENTIFIER));
            string_t name = tstream_token(ts)->repr;
            rspan = tstream_token(ts)->span;
            tstream_advance(ts);

            tmp = expr_alloc();
            tmp->kind = EXPR_ACCESS;
            tmp->access.expr = e;
            tmp->access.op.kind = EXPR_ACCESS_DOT;
            tmp->access.op.span = op_span;
            tmp->access.field.name = name;
            tmp->access.field.span = rspan;
            tmp->span = concat_span(e->span, rspan);
            e = tmp;
        } else if (tstream_expect(ts, TK_ARROW, &op_span)) {
            TRY(check_kind(ctx, ts, TK_IDENTIFIER));
            string_t name = tstream_token(ts)->repr;
            rspan = tstream_token(ts)->span;
            tstream_advance(ts);

            tmp = expr_alloc();
            tmp->kind = EXPR_ACCESS;
            tmp->access.expr = e;
            tmp->access.op.kind = EXPR_ACCESS_ARROW;
            tmp->access.op.span = op_span;
            tmp->access.field.name = name;
            tmp->access.field.span = rspan;
            tmp->span = concat_span(e->span, rspan);
            e = tmp;
        } else if (tstream_expect(ts, TK_PLUSPLUS, &op_span)) {
            tmp = expr_alloc();
            tmp->kind = EXPR_POSTFIX;
            tmp->postfix.op.kind = EXPR_POSTFIX_INC;
            tmp->postfix.op.span = op_span;
            tmp->postfix.expr = e;
            tmp->span = concat_span(e->span, rspan);
            e = tmp;
        } else if (tstream_expect(ts, TK_MINUSMINUS, &op_span)) {
            tmp = expr_alloc();
            tmp->kind = EXPR_POSTFIX;
            tmp->postfix.op.kind = EXPR_POSTFIX_DEC;
            tmp->postfix.op.span = op_span;
            tmp->postfix.expr = e;
            tmp->span = concat_span(e->span, rspan);
            e = tmp;
        } else {
            break;
        }
    }
    memcpy(expr, e, sizeof(expr_t));
    return true;
}

bool parse_primary_expr(context_t *ctx, tstream_t *ts, expr_t *expr) {
    TRY(check_kinds(ctx, ts, 5, TK_IDENTIFIER, TK_INTEGER, TK_FLOATING,
                    TK_STRING, TK_CHARACTER, TK_LPAREN));
    token_t *token = tstream_token(ts);
    tstream_advance(ts);
    if (token->kind == TK_IDENTIFIER) {
        expr->kind = EXPR_IDENT;
        expr->ident.value = token->repr;
        expr->span = token->span;
        return true;
    } else if (token->kind == TK_INTEGER) {
        expr->kind = EXPR_INTEGER;
        expr->integer.value = strtoull(token_integer_body(token).str, NULL, 0);
        expr->integer.radix = token->int_.radix;
        expr->integer.suffix = token->int_.suffix;
        expr->span = token->span;
        return true;
    } else if (token->kind == TK_FLOATING) {
        expr->kind = EXPR_FLOATING;
        expr->floating.value = strtod(token_floating_body(token).str, NULL);
        expr->floating.radix = token->float_.radix;
        expr->floating.suffix = token->float_.suffix;
        expr->span = token->span;
        return true;
    } else if (token->kind == TK_STRING) {
        expr->kind = EXPR_STRING;
        expr->string.value = token_string_body(token);
        return true;
    } else if (token->kind == TK_CHARACTER) {
        expr->kind = EXPR_CHARACTER;
        expr->character.value = token_character_body(token);
        return true;
    } else {
        span_t rspan;
        TRY(parse_expr(ctx, ts, expr));
        TRY(expect_kind(ctx, ts, TK_RPAREN, &rspan));
        expr->span = concat_span(token->span, rspan);
        return true;
    }
}
