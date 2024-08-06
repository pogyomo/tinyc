#include "decl.h"

#include <assert.h>

#include "../report.h"
#include "ast/decl.h"
#include "ast/type.h"
#include "context.h"
#include "expr.h"
#include "stream.h"
#include "type.h"

static bool parse_designators(struct parse_context *ctx, struct tstream *ts,
                              struct designator **desigs) {
    struct designator head = {NULL};
    struct designator *prev = &head;
    while (true) {
        if (tstream_is_punct(ts, TK_PUNCT_DOT)) {
            struct span span = tstream_curr(ts)->span;
            tstream_advance(ts);

            TRY(tstream_check_ident(ctx->ctx, ts));
            char *ident = tstream_curr(ts)->ident.value.str;
            merge_span(&span, &tstream_curr(ts)->span, &span);
            tstream_advance(ts);

            prev->next = designator_ident_new(ident, &span);
            prev = prev->next;
        } else if (tstream_is_punct(ts, TK_PUNCT_LSQUARE)) {
            struct span span = tstream_curr(ts)->span;
            tstream_advance(ts);

            struct expr *size;
            TRY(parse_const_expr(ctx, ts, &size));

            TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_RSQUARE));
            merge_span(&span, &tstream_curr(ts)->span, &span);
            tstream_advance(ts);

            prev->next = designator_index_new(size, &span);
            prev = prev->next;
        } else {
            break;
        }
    }
    *desigs = head.next;
    return true;
}

bool parse_initializer(struct parse_context *ctx, struct tstream *ts,
                       struct initializer **init) {
    struct span span;

    if (!tstream_is_punct(ts, TK_PUNCT_LCURLY)) {
        struct expr *expr;
        TRY(parse_assign_expr(ctx, ts, &expr));
        *init = initializer_expr_new(expr, &expr->span);
        return true;
    }
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    struct initializer_list_item head;
    struct initializer_list_item *prev = &head;
    while (true) {
        if (tstream_is_punct(ts, TK_PUNCT_RCURLY)) {
            break;
        }

        TRY(tstream_check_eos(ctx->ctx, ts));
        struct span span = tstream_curr(ts)->span;

        struct designator *desigs;
        TRY(parse_designators(ctx, ts, &desigs));

        struct initializer *value = NULL;
        if (desigs) {
            TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_ASSIGN));
            tstream_advance(ts);
        }
        TRY(parse_initializer(ctx, ts, &value));
        merge_span(&span, &value->span, &span);

        prev->next = initializer_list_item_new(desigs, value, &span);
        prev = prev->next;

        if (tstream_is_punct(ts, TK_PUNCT_RCURLY)) {
            break;
        } else if (tstream_is_punct(ts, TK_PUNCT_COMMA)) {
            tstream_advance(ts);
        } else {
            struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                       "expected `}` or `,` after this", ""};
            report(ctx->ctx, &info);
            return false;
        }
    }
    merge_span(&span, &tstream_curr(ts)->span, &span);
    tstream_advance(ts);

    struct initializer_list list = {head.next};
    *init = initializer_list_new(&list, &span);
    return true;
}

static bool convert_func_type_to_decl_fun(struct parse_context *ctx,
                                          struct storage_class *class,
                                          struct function_spec *func_spec,
                                          struct type *type,
                                          struct type_rest_name *name,
                                          struct decl **decl) {
    assert(type->kind == TYPE_FUNC);

    struct decl_fun_param head = {NULL};
    struct decl_fun_param *prev = &head;
    for (struct type_func_param *param = type->func.params; param;
         param = param->next) {
        if (param->kind == TYPE_FUNC_PARAM_NORMAL) {
            struct decl_fun_param_name name = {
                param->name.exists, param->name.value, param->name.span};
            prev->next =
                decl_fun_param_normal_new(param->type, &name, &param->span);
        } else {
            assert(param->next == NULL);
            prev->next = decl_fun_param_varidic_new(&param->span);
        }
        prev = prev->next;
    }

    if (!name->exists) {
        struct report_info info = {REPORT_ERROR, type->span,
                                   "function declaration name expected", ""};
        report(ctx->ctx, &info);
        return false;
    }
    struct decl_fun_name name_ = {name->name, name->span};
    *decl = decl_fun_new(type->func.ret_type, class, func_spec, &name_,
                         head.next, &type->span);
    return true;
}

static bool convert_type_to_decl_var(
    struct parse_context *ctx, struct storage_class *class,
    struct function_spec *func_spec, struct type *type,
    struct type_rest_name *name, struct initializer *init, struct decl **decl) {
    if (func_spec) {
        struct report_info info = {REPORT_ERROR, func_spec->span,
                                   "function specifier is not expected", ""};
        report(ctx->ctx, &info);
        return false;
    }

    if (!name->exists) {
        struct report_info info = {REPORT_ERROR, type->span,
                                   "variable declaration name expected", ""};
        report(ctx->ctx, &info);
        return false;
    }
    struct decl_var_name name_ = {name->name, name->span};
    *decl = decl_var_new(type, class, &name_, init, &type->span);
    return true;
}

bool parse_decl(struct parse_context *ctx, struct tstream *ts,
                struct decl **decl) {
    struct type *head = NULL;
    struct storage_class *class = NULL;
    struct function_spec *func_spec = NULL;
    TRY(parse_type_head(ctx, ts, &head, &class, &func_spec));

    struct decl decl_head = {NULL};
    struct decl *decl_prev = &decl_head;
    bool is_first = true;
    while (true) {
        if (is_first && tstream_is_punct(ts, TK_PUNCT_SEMICOLON)) {
            break;
        } else {
            struct type *type;
            struct type_rest_name name;
            TRY(parse_type_rest(ctx, ts, head, &type, &name));

            if (type->kind == TYPE_FUNC) {
                TRY(convert_func_type_to_decl_fun(ctx, class, func_spec, type,
                                                  &name, &decl_prev->next));
            } else {
                struct initializer *init = NULL;
                if (tstream_is_punct(ts, TK_PUNCT_ASSIGN)) {
                    tstream_advance(ts);
                    TRY(parse_initializer(ctx, ts, &init));
                }
                TRY(convert_type_to_decl_var(ctx, class, func_spec, type, &name,
                                             init, &decl_prev->next));
            }
            decl_prev = decl_prev->next;

            if (class && class->kind == STORAGE_CLASS_TYPEDEF && name.exists) {
                parse_context_insert_typedef_name(ctx, name.name);
            }

            if (tstream_is_punct(ts, TK_PUNCT_SEMICOLON)) {
                break;
            } else if (tstream_is_punct(ts, TK_PUNCT_COMMA)) {
                tstream_advance(ts);
            } else {
                struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                           "expected `,` or `;` after this",
                                           ""};
                report(ctx->ctx, &info);
                return false;
            }
        }
        is_first = false;
    }
    tstream_advance(ts);

    *decl = decl_head.next;
    return true;
}
