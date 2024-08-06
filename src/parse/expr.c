#include "expr.h"

#include <stdlib.h>
#include <string.h>

#include "../report.h"
#include "ast/expr.h"
#include "stream.h"
#include "type.h"

bool parse_expr(struct parse_context *ctx, struct tstream *ts,
                struct expr **expr) {
    struct expr *lhs;
    TRY(parse_assign_expr(ctx, ts, &lhs));
    while (!tstream_eos(ts)) {
        if (tstream_is_punct(ts, TK_PUNCT_COMMA)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_COMMA,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_assign_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else {
            break;
        }
    }
    *expr = lhs;
    return true;
}

bool parse_const_expr(struct parse_context *ctx, struct tstream *ts,
                      struct expr **expr) {
    return parse_cond_expr(ctx, ts, expr);
}

bool parse_assign_expr(struct parse_context *ctx, struct tstream *ts,
                       struct expr **expr) {
    static struct {
        enum token_punct_kind punct_kind;
        enum expr_assign_op_kind op_kind;
    } punct_to_op[] = {
        {TK_PUNCT_ASSIGN, EXPR_ASSIGN_OP_NORMAL},
        {TK_PUNCT_ADDASSIGN, EXPR_ASSIGN_OP_ADD},
        {TK_PUNCT_SUBASSIGN, EXPR_ASSIGN_OP_SUB},
        {TK_PUNCT_MULASSIGN, EXPR_ASSIGN_OP_MUL},
        {TK_PUNCT_DIVASSIGN, EXPR_ASSIGN_OP_DIV},
        {TK_PUNCT_MODASSIGN, EXPR_ASSIGN_OP_MOD},
        {TK_PUNCT_LSHIFTASSIGN, EXPR_ASSIGN_OP_LSHIFT},
        {TK_PUNCT_RSHIFTASSIGN, EXPR_ASSIGN_OP_RSHIFT},
        {TK_PUNCT_ANDASSIGN, EXPR_ASSIGN_OP_BITAND},
        {TK_PUNCT_ORASSIGN, EXPR_ASSIGN_OP_BITOR},
        {TK_PUNCT_XORASSIGN, EXPR_ASSIGN_OP_BITXOR},
    };

    struct tstream ts_save = *ts;

    context_suppress_report(ctx->ctx);

    struct expr *lhs;
    if (!parse_unary_expr(ctx, ts, &lhs)) {
        *ts = ts_save;
        context_activate_report(ctx->ctx);
        return parse_cond_expr(ctx, ts, expr);
    }

    context_activate_report(ctx->ctx);

    struct expr_assign_op op;
    bool found = false;
    for (size_t i = 0; i < sizeof punct_to_op / sizeof punct_to_op[0]; i++) {
        if (tstream_is_punct(ts, punct_to_op[i].punct_kind)) {
            op.kind = punct_to_op[i].op_kind;
            op.span = tstream_curr(ts)->span;
            found = true;
            break;
        }
    }
    if (!found) {
        *ts = ts_save;
        return parse_cond_expr(ctx, ts, expr);
    }

    struct expr *rhs;
    TRY(parse_assign_expr(ctx, ts, &rhs));

    struct span span;
    merge_span(&lhs->span, &rhs->span, &span);
    *expr = expr_assign_new(lhs, rhs, &op, &span);
    return true;
}

bool parse_cond_expr(struct parse_context *ctx, struct tstream *ts,
                     struct expr **expr) {
    struct expr *cond;
    TRY(parse_logical_or_expr(ctx, ts, &cond));

    if (!tstream_is_punct(ts, TK_PUNCT_QUESTION)) {
        *expr = cond;
        return true;
    }
    tstream_advance(ts);

    struct expr *then;
    TRY(parse_expr(ctx, ts, &then));

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_COLON));

    struct expr *else_;
    TRY(parse_cond_expr(ctx, ts, &else_));

    struct span span;
    merge_span(&cond->span, &else_->span, &span);
    *expr = expr_cond_new(cond, then, else_, &span);

    return true;
}

bool parse_logical_or_expr(struct parse_context *ctx, struct tstream *ts,
                           struct expr **expr) {
    struct expr *lhs;
    TRY(parse_logical_and_expr(ctx, ts, &lhs));
    while (!tstream_eos(ts)) {
        if (tstream_is_punct(ts, TK_PUNCT_OR)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_OR,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_logical_or_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else {
            break;
        }
    }
    *expr = lhs;
    return true;
}

bool parse_logical_and_expr(struct parse_context *ctx, struct tstream *ts,
                            struct expr **expr) {
    struct expr *lhs;
    TRY(parse_inclusive_or_expr(ctx, ts, &lhs));
    while (!tstream_eos(ts)) {
        if (tstream_is_punct(ts, TK_PUNCT_AND)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_AND,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_logical_and_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else {
            break;
        }
    }
    *expr = lhs;
    return true;
}

bool parse_inclusive_or_expr(struct parse_context *ctx, struct tstream *ts,
                             struct expr **expr) {
    struct expr *lhs;
    TRY(parse_exclusive_or_expr(ctx, ts, &lhs));
    while (!tstream_eos(ts)) {
        if (tstream_is_punct(ts, TK_PUNCT_VERTICAL)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_BITOR,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_inclusive_or_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else {
            break;
        }
    }
    *expr = lhs;
    return true;
}

bool parse_exclusive_or_expr(struct parse_context *ctx, struct tstream *ts,
                             struct expr **expr) {
    struct expr *lhs;
    TRY(parse_and_expr(ctx, ts, &lhs));
    while (!tstream_eos(ts)) {
        if (tstream_is_punct(ts, TK_PUNCT_HAT)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_BITXOR,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_exclusive_or_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else {
            break;
        }
    }
    *expr = lhs;
    return true;
}

bool parse_and_expr(struct parse_context *ctx, struct tstream *ts,
                    struct expr **expr) {
    struct expr *lhs;
    TRY(parse_equality_expr(ctx, ts, &lhs));
    while (!tstream_eos(ts)) {
        if (tstream_is_punct(ts, TK_PUNCT_AMPERSAND)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_BITAND,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_and_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else {
            break;
        }
    }
    *expr = lhs;
    return true;
}

bool parse_equality_expr(struct parse_context *ctx, struct tstream *ts,
                         struct expr **expr) {
    struct expr *lhs;
    TRY(parse_relational_expr(ctx, ts, &lhs));
    while (!tstream_eos(ts)) {
        if (tstream_is_punct(ts, TK_PUNCT_EQ)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_EQ,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_equality_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else if (tstream_is_punct(ts, TK_PUNCT_NE)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_NE,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_equality_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else {
            break;
        }
    }
    *expr = lhs;
    return true;
}

bool parse_relational_expr(struct parse_context *ctx, struct tstream *ts,
                           struct expr **expr) {
    struct expr *lhs;
    TRY(parse_shift_expr(ctx, ts, &lhs));
    while (!tstream_eos(ts)) {
        if (tstream_is_punct(ts, TK_PUNCT_LT)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_LT,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_relational_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else if (tstream_is_punct(ts, TK_PUNCT_GT)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_GT,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_relational_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else if (tstream_is_punct(ts, TK_PUNCT_LE)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_LE,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_relational_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else if (tstream_is_punct(ts, TK_PUNCT_GE)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_GE,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_relational_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else {
            break;
        }
    }
    *expr = lhs;
    return true;
}

bool parse_shift_expr(struct parse_context *ctx, struct tstream *ts,
                      struct expr **expr) {
    struct expr *lhs;
    TRY(parse_additive_expr(ctx, ts, &lhs));
    while (!tstream_eos(ts)) {
        if (tstream_is_punct(ts, TK_PUNCT_LSHIFT)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_LSHIFT,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_shift_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else if (tstream_is_punct(ts, TK_PUNCT_RSHIFT)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_RSHIFT,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_shift_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else {
            break;
        }
    }
    *expr = lhs;
    return true;
}

bool parse_additive_expr(struct parse_context *ctx, struct tstream *ts,
                         struct expr **expr) {
    struct expr *lhs;
    TRY(parse_multiplicative_expr(ctx, ts, &lhs));
    while (!tstream_eos(ts)) {
        if (tstream_is_punct(ts, TK_PUNCT_PLUS)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_ADD,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_additive_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else if (tstream_is_punct(ts, TK_PUNCT_MINUS)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_SUB,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_additive_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else {
            break;
        }
    }
    *expr = lhs;
    return true;
}

bool parse_multiplicative_expr(struct parse_context *ctx, struct tstream *ts,
                               struct expr **expr) {
    struct expr *lhs;
    TRY(parse_cast_expr(ctx, ts, &lhs));
    while (!tstream_eos(ts)) {
        if (tstream_is_punct(ts, TK_PUNCT_STAR)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_MUL,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_multiplicative_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else if (tstream_is_punct(ts, TK_PUNCT_SLASH)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_DIV,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_multiplicative_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else if (tstream_is_punct(ts, TK_PUNCT_PERCENT)) {
            struct expr_infix_op op = {EXPR_INFIX_OP_MOD,
                                       tstream_curr(ts)->span};
            tstream_advance(ts);

            struct expr *rhs;
            TRY(parse_multiplicative_expr(ctx, ts, &rhs));

            struct span span;
            merge_span(&lhs->span, &rhs->span, &span);
            lhs = expr_infix_new(lhs, rhs, &op, &span);
        } else {
            break;
        }
    }
    *expr = lhs;
    return true;
}

bool parse_cast_expr(struct parse_context *ctx, struct tstream *ts,
                     struct expr **expr) {
    if (tstream_is_punct(ts, TK_PUNCT_LPAREN)) {
        struct span span = tstream_curr(ts)->span;
        struct tstream ts_save = *ts;
        tstream_advance(ts);

        context_suppress_report(ctx->ctx);

        struct type *type;
        if (!parse_type_name(ctx, ts, &type)) {
            context_activate_report(ctx->ctx);
            *ts = ts_save;
            return parse_unary_expr(ctx, ts, expr);
        }

        context_activate_report(ctx->ctx);
        if (!tstream_is_punct(ts, TK_PUNCT_RPAREN)) {
            *ts = ts_save;
            return parse_unary_expr(ctx, ts, expr);
        }
        tstream_advance(ts);

        TRY(parse_cast_expr(ctx, ts, expr));
        merge_span(&span, &(*expr)->span, &span);

        *expr = expr_cast_new(type, *expr, &span);
        return true;
    } else {
        return parse_unary_expr(ctx, ts, expr);
    }
}

bool parse_unary_expr(struct parse_context *ctx, struct tstream *ts,
                      struct expr **expr) {
    if (tstream_is_punct(ts, TK_PUNCT_PLUSPLUS)) {
        struct expr_unary_op op = {EXPR_UNARY_OP_INC, tstream_curr(ts)->span};
        tstream_advance(ts);

        TRY(parse_unary_expr(ctx, ts, expr));

        struct span span;
        merge_span(&op.span, &(*expr)->span, &span);
        *expr = expr_unary_new(*expr, &op, &span);
        return true;
    } else if (tstream_is_punct(ts, TK_PUNCT_MINUSMINUS)) {
        struct expr_unary_op op = {EXPR_UNARY_OP_DEC, tstream_curr(ts)->span};
        tstream_advance(ts);

        TRY(parse_unary_expr(ctx, ts, expr));

        struct span span;
        merge_span(&op.span, &(*expr)->span, &span);
        *expr = expr_unary_new(*expr, &op, &span);
        return true;
    } else if (tstream_is_punct(ts, TK_PUNCT_AMPERSAND)) {
        struct expr_unary_op op = {EXPR_UNARY_OP_REF, tstream_curr(ts)->span};
        tstream_advance(ts);

        TRY(parse_cast_expr(ctx, ts, expr));

        struct span span;
        merge_span(&op.span, &(*expr)->span, &span);
        *expr = expr_unary_new(*expr, &op, &span);
        return true;
    } else if (tstream_is_punct(ts, TK_PUNCT_STAR)) {
        struct expr_unary_op op = {EXPR_UNARY_OP_DEREF, tstream_curr(ts)->span};
        tstream_advance(ts);

        TRY(parse_cast_expr(ctx, ts, expr));

        struct span span;
        merge_span(&op.span, &(*expr)->span, &span);
        *expr = expr_unary_new(*expr, &op, &span);
        return true;
    } else if (tstream_is_punct(ts, TK_PUNCT_PLUS)) {
        struct expr_unary_op op = {EXPR_UNARY_OP_PLUS, tstream_curr(ts)->span};
        tstream_advance(ts);

        TRY(parse_cast_expr(ctx, ts, expr));

        struct span span;
        merge_span(&op.span, &(*expr)->span, &span);
        *expr = expr_unary_new(*expr, &op, &span);
        return true;
    } else if (tstream_is_punct(ts, TK_PUNCT_MINUS)) {
        struct expr_unary_op op = {EXPR_UNARY_OP_MINUS, tstream_curr(ts)->span};
        tstream_advance(ts);

        TRY(parse_cast_expr(ctx, ts, expr));

        struct span span;
        merge_span(&op.span, &(*expr)->span, &span);
        *expr = expr_unary_new(*expr, &op, &span);
        return true;
    } else if (tstream_is_punct(ts, TK_PUNCT_TILDE)) {
        struct expr_unary_op op = {EXPR_UNARY_OP_INV, tstream_curr(ts)->span};
        tstream_advance(ts);

        TRY(parse_cast_expr(ctx, ts, expr));

        struct span span;
        merge_span(&op.span, &(*expr)->span, &span);
        *expr = expr_unary_new(*expr, &op, &span);
        return true;
    } else if (tstream_is_punct(ts, TK_PUNCT_NOT)) {
        struct expr_unary_op op = {EXPR_UNARY_OP_NEG, tstream_curr(ts)->span};
        tstream_advance(ts);

        TRY(parse_cast_expr(ctx, ts, expr));

        struct span span;
        merge_span(&op.span, &(*expr)->span, &span);
        *expr = expr_unary_new(*expr, &op, &span);
        return true;
    } else if (tstream_is_keyword(ts, TK_KEYWORD_SIZEOF)) {
        struct span span = tstream_curr(ts)->span;
        tstream_advance(ts);
        if (tstream_is_punct(ts, TK_PUNCT_LPAREN)) {
            struct tstream ts_save = *ts;
            tstream_advance(ts);

            context_suppress_report(ctx->ctx);

            struct type *type;
            if (!parse_type_name(ctx, ts, &type)) {
                context_activate_report(ctx->ctx);
                *ts = ts_save;
                TRY(parse_unary_expr(ctx, ts, expr));
                merge_span(&span, &(*expr)->span, &span);
                *expr = expr_sizeof_expr_new(*expr, &span);
                return true;
            }

            context_activate_report(ctx->ctx);
            if (!tstream_is_punct(ts, TK_PUNCT_RPAREN)) {
                *ts = ts_save;
                TRY(parse_unary_expr(ctx, ts, expr));
                merge_span(&span, &(*expr)->span, &span);
                *expr = expr_sizeof_expr_new(*expr, &span);
                return true;
            }
            merge_span(&span, &tstream_curr(ts)->span, &span);
            tstream_advance(ts);

            *expr = expr_sizeof_type_new(type, &span);
            return true;
        } else {
            context_activate_report(ctx->ctx);
            TRY(parse_unary_expr(ctx, ts, expr));
            merge_span(&span, &(*expr)->span, &span);
            *expr = expr_sizeof_expr_new(*expr, &span);
            return true;
        }
    } else if (tstream_is_ident(ts) &&
               strcmp(tstream_curr(ts)->ident.value.str, "defined") == 0 &&
               ctx->in_preprocess) {
        struct span span = tstream_curr(ts)->span;
        tstream_advance(ts);

        bool has_paren = false;
        if (tstream_is_punct(ts, TK_PUNCT_LPAREN)) {
            has_paren = true;
            tstream_advance(ts);
        }

        if (!tstream_is_ident(ts)) {
            struct report_info info = {REPORT_ERROR, tstream_curr(ts)->span,
                                       "identifier expected inside defined",
                                       ""};
            report(ctx->ctx, &info);
            return false;
        }
        struct expr_defined_ident ident = {tstream_curr(ts)->ident.value.str,
                                           tstream_curr(ts)->span};
        merge_span(&span, &ident.span, &span);
        tstream_advance(ts);

        if (has_paren) {
            if (!tstream_is_punct(ts, TK_PUNCT_RPAREN)) {
                struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                           "`)` expected after this", ""};
                report(ctx->ctx, &info);
                return false;
            }
            merge_span(&span, &tstream_curr(ts)->span, &span);
            tstream_advance(ts);
        }

        *expr = expr_defined_new(&ident, &span);
        return true;
    } else {
        return parse_postfix_expr(ctx, ts, expr);
    }
}

bool parse_postfix_expr(struct parse_context *ctx, struct tstream *ts,
                        struct expr **expr) {
    struct expr *lhs;
    TRY(parse_primary_expr(ctx, ts, &lhs));
    while (!tstream_eos(ts)) {
        if (tstream_is_punct(ts, TK_PUNCT_LSQUARE)) {
            tstream_advance(ts);

            struct expr *index;
            TRY(parse_expr(ctx, ts, &index));

            TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_RSQUARE));
            struct span span;
            merge_span(&lhs->span, &tstream_curr(ts)->span, &span);
            tstream_advance(ts);

            lhs = expr_index_new(lhs, index, &span);
        } else if (tstream_is_punct(ts, TK_PUNCT_LPAREN)) {
            tstream_advance(ts);

            if (tstream_is_punct(ts, TK_PUNCT_RPAREN)) {
                struct span span;
                merge_span(&lhs->span, &tstream_curr(ts)->span, &span);
                tstream_advance(ts);

                lhs = expr_call_new(lhs, NULL, &span);
                continue;
            }

            struct expr *arg_value;
            TRY(parse_assign_expr(ctx, ts, &arg_value));
            struct expr_call_arg *arg_head = expr_call_arg_new(arg_value);
            struct expr_call_arg *arg_prev = arg_head;

            while (true) {
                TRY(tstream_check_eos(ctx->ctx, ts));
                if (tstream_is_punct(ts, TK_PUNCT_COMMA)) {
                    struct expr *arg_value;
                    TRY(parse_assign_expr(ctx, ts, &arg_value));

                    arg_prev->next = expr_call_arg_new(arg_value);
                    arg_prev = arg_prev->next;
                } else {
                    break;
                }
            }

            TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_RPAREN));
            struct span span;
            merge_span(&lhs->span, &tstream_curr(ts)->span, &span);
            tstream_advance(ts);

            lhs = expr_call_new(lhs, arg_head, &span);
        } else if (tstream_is_punct(ts, TK_PUNCT_DOT)) {
            struct expr_access_op op = {EXPR_ACCESS_OP_DOT,
                                        tstream_curr(ts)->span};
            tstream_advance(ts);

            TRY(tstream_check_ident(ctx->ctx, ts));
            struct expr_access_field field = {tstream_curr(ts)->ident.value,
                                              tstream_curr(ts)->span};
            tstream_advance(ts);

            struct span span;
            merge_span(&lhs->span, &field.span, &span);
            lhs = expr_access_new(lhs, &op, &field, &span);
        } else if (tstream_is_punct(ts, TK_PUNCT_ARROW)) {
            struct expr_access_op op = {EXPR_ACCESS_OP_ARROW,
                                        tstream_curr(ts)->span};
            tstream_advance(ts);

            TRY(tstream_check_ident(ctx->ctx, ts));
            struct expr_access_field field = {tstream_curr(ts)->ident.value,
                                              tstream_curr(ts)->span};
            tstream_advance(ts);

            struct span span;
            merge_span(&lhs->span, &field.span, &span);
            lhs = expr_access_new(lhs, &op, &field, &span);
        } else if (tstream_is_punct(ts, TK_PUNCT_PLUSPLUS)) {
            struct expr_postfix_op op = {EXPR_POSTFIX_OP_INC,
                                         tstream_curr(ts)->span};
            tstream_advance(ts);

            struct span span;
            merge_span(&lhs->span, &op.span, &span);
            lhs = expr_postfix_new(lhs, &op, &span);
        } else if (tstream_is_punct(ts, TK_PUNCT_MINUSMINUS)) {
            struct expr_postfix_op op = {EXPR_POSTFIX_OP_DEC,
                                         tstream_curr(ts)->span};
            tstream_advance(ts);

            struct span span;
            merge_span(&lhs->span, &op.span, &span);
            lhs = expr_postfix_new(lhs, &op, &span);
        } else {
            break;
        }
    }
    *expr = lhs;
    return true;
}

bool parse_primary_expr(struct parse_context *ctx, struct tstream *ts,
                        struct expr **expr) {
    if (tstream_is_ident(ts)) {
        *expr = expr_ident_new(&tstream_curr(ts)->ident.value,
                               &tstream_curr(ts)->span);
        tstream_advance(ts);
        return true;
    } else if (tstream_is_int(ts)) {
        *expr =
            expr_int_new(tstream_curr(ts)->int_.value, &tstream_curr(ts)->span);
        tstream_advance(ts);
        return true;
    } else if (tstream_is_float(ts)) {
        *expr = expr_float_new(tstream_curr(ts)->float_.value,
                               &tstream_curr(ts)->span);
        tstream_advance(ts);
        return true;
    } else if (tstream_is_char(ts)) {
        *expr = expr_char_new(&tstream_curr(ts)->char_.value,
                              &tstream_curr(ts)->span);
        tstream_advance(ts);
        return true;
    } else if (tstream_is_string(ts)) {
        *expr = expr_string_new(&tstream_curr(ts)->string.value,
                                &tstream_curr(ts)->span);
        tstream_advance(ts);
        return true;
    } else if (tstream_is_punct(ts, TK_PUNCT_LPAREN)) {
        tstream_advance(ts);
        TRY(parse_expr(ctx, ts, expr));
        TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_RPAREN));
        tstream_advance(ts);
        return true;
    } else {
        TRY(tstream_check_eos(ctx->ctx, ts));
        struct report_info info = {REPORT_ERROR, tstream_curr(ts)->span,
                                   "expexted primary expression here", ""};
        report(ctx->ctx, &info);
        return false;
    }
}
