#include "fun_def.h"

#include <assert.h>

#include "../report.h"
#include "ast/decl.h"
#include "ast/fun_def.h"
#include "ast/type.h"
#include "stmt.h"
#include "stream.h"
#include "type.h"

bool parse_fun_def(struct parse_context *ctx, struct tstream *ts,
                   struct fun_def **def, bool *fallback) {
    *fallback = true;

    struct type *type;
    struct type_rest_name name;
    struct function_spec *fun_spec;

    TRY(parse_type_head(ctx, ts, &type, NULL, &fun_spec));
    TRY(parse_type_rest(ctx, ts, type, &type, &name));

    if (type->kind != TYPE_FUNC) return false;

    struct fun_def_param head = {NULL};
    struct fun_def_param *prev = &head;
    for (struct type_func_param *param = type->func.params; param;
         param = param->next) {
        if (param->kind == TYPE_FUNC_PARAM_NORMAL) {
            struct fun_def_param_name name_ = {name.exists, name.name,
                                               name.span};
            prev->next =
                fun_def_param_normal_new(param->type, &name_, &param->span);
            prev = prev->next;
        } else {
            assert(param->next == NULL);
            prev->next = fun_def_param_varidic_new(&param->span);
            prev = prev->next;
        }
    }

    if (!tstream_is_punct(ts, TK_PUNCT_LCURLY)) {
        return false;
    }
    *fallback = false;

    struct stmt *body;
    TRY(parse_block_stmt(ctx, ts, &body));

    struct span span;
    merge_span(&type->span, &body->span, &span);

    if (!name.exists) {
        struct report_info info = {REPORT_ERROR, type->span,
                                   "expected function name", ""};
        report(ctx->ctx, &info);
        return false;
    }
    struct fun_def_name name_ = {name.name, name.span};

    *def = fun_def_new(type->func.ret_type, &name_, head.next, body, &span);

    return true;
}
