#include "parser_decl.h"

#include <stdarg.h>

#include "../memory.h"
#include "../panic.h"
#include "../report.h"
#include "ast.h"
#include "parser_expr.h"
#include "utils.h"

static int tk_cmp(const void *a, const void *b) {
    return (int)((token_t *)a)->kind - (int)((token_t *)b)->kind;
}

// Parse struct specifier.
static bool parse_struct(context_t *ctx, tstream_t *ts, type_t *type) {
    span_t first, last;
    TRY(expect_kind(ctx, ts, TK_STRUCT, &first));
    last = first;

    string_t *name = NULL;
    if (tstream_is(ts, TK_IDENTIFIER)) {
        name = malloc_panic(sizeof(string_t));
        *name = tstream_token(ts)->repr;
        last = tstream_token(ts)->span;
        tstream_advance(ts);
    }

    if (!tstream_expect(ts, TK_LCURLY, NULL)) {
        type->kind = TYPE_STRUCT;
        type->struct_.fields = NULL;
        type->struct_.name = name;
        type->span = concat_span(first, last);
    }

    VECTOR_REF(decl_t *) fields = malloc_panic(sizeof(vector_t));
    vector_init(fields, sizeof(decl_t *));
    while (true) {
        TRY(check_eos(ctx, ts));
        if (tstream_expect(ts, TK_RCURLY, &last)) {
            break;
        }
        VECTOR(decl_t *) field;
        parse_decls(ctx, ts, &field);
        vector_append(fields, &field);
    }

    type->kind = TYPE_STRUCT;
    type->struct_.fields = fields;
    type->struct_.name = name;
    type->span = concat_span(first, last);
    return true;
}

// Parse union specifier.
static bool parse_union(context_t *ctx, tstream_t *ts, type_t *type) {
    span_t first, last;
    TRY(expect_kind(ctx, ts, TK_UNION, &first));
    last = first;

    string_t *name = NULL;
    if (tstream_is(ts, TK_IDENTIFIER)) {
        name = malloc_panic(sizeof(string_t));
        *name = tstream_token(ts)->repr;
        last = tstream_token(ts)->span;
        tstream_advance(ts);
    }

    if (!tstream_expect(ts, TK_LCURLY, NULL)) {
        type->kind = TYPE_UNION;
        type->union_.fields = NULL;
        type->union_.name = name;
        type->span = concat_span(first, last);
    }

    VECTOR_REF(decl_t *) fields = malloc_panic(sizeof(vector_t));
    vector_init(fields, sizeof(decl_t *));
    while (true) {
        TRY(check_eos(ctx, ts));
        if (tstream_expect(ts, TK_RCURLY, &last)) {
            break;
        }
        VECTOR(decl_t *) field;
        parse_decls(ctx, ts, &field);
        vector_append(fields, &field);
    }

    type->kind = TYPE_UNION;
    type->union_.fields = fields;
    type->union_.name = name;
    type->span = concat_span(first, last);
    return true;
}

// Parse enum specifier
static bool parse_enum(context_t *ctx, tstream_t *ts, type_t *type) {
    span_t first, last;
    TRY(expect_kind(ctx, ts, TK_ENUM, &first));
    last = first;

    string_t *name = NULL;
    if (tstream_is(ts, TK_IDENTIFIER)) {
        name = malloc_panic(sizeof(string_t));
        *name = tstream_token(ts)->repr;
        last = tstream_token(ts)->span;
        tstream_advance(ts);
    }

    if (!tstream_expect(ts, TK_LCURLY, NULL)) {
        type->kind = TYPE_UNION;
        type->union_.fields = NULL;
        type->union_.name = name;
        type->span = concat_span(first, last);
    }

    VECTOR_REF(enumerator_t) fields = malloc_panic(sizeof(vector_t));
    vector_init(fields, sizeof(enumerator_t));
    while (true) {
        TRY(check_eos(ctx, ts));
        if (tstream_expect(ts, TK_RCURLY, &last)) {
            break;
        }

        span_t first, last;
        TRY(check_kind(ctx, ts, TK_IDENTIFIER));
        string_t name = tstream_token(ts)->repr;
        first = last = tstream_token(ts)->span;
        tstream_advance(ts);

        expr_t *value = NULL;
        if (tstream_expect(ts, TK_ASSIGN, NULL)) {
            value = expr_alloc();
            TRY(parse_expr(ctx, ts, value));
            TRY(expect_kind(ctx, ts, TK_SEMICOLON, &last));
        }
        enumerator_t enumerator = {name, value, concat_span(first, last)};
        vector_push(fields, &enumerator);
    }

    type->kind = TYPE_UNION;
    type->union_.fields = fields;
    type->union_.name = name;
    type->span = concat_span(first, last);
    return true;
}

static bool collect_type_infos(context_t *ctx, tstream_t *ts, type_t **struct_,
                               type_t **union_, type_t **enum_,
                               VECTOR_REF(token_t) sspecs,
                               VECTOR_REF(token_t) tspecs,
                               VECTOR_REF(token_t) tquals,
                               VECTOR_REF(token_t) fspecs) {
    *struct_ = NULL;
    *union_ = NULL;
    *enum_ = NULL;
    vector_init(sspecs, sizeof(token_t));
    vector_init(tspecs, sizeof(token_t));
    vector_init(tquals, sizeof(token_t));
    vector_init(fspecs, sizeof(token_t));

    token_t *token;
    while (!tstream_eos(ts)) {
        token = tstream_token(ts);
        if (token->kind == TK_TYPEDEF || token->kind == TK_EXTERN ||
            token->kind == TK_STATIC || token->kind == TK_AUTO ||
            token->kind == TK_REGISTER) {
            vector_push(sspecs, token);
            tstream_advance(ts);
        } else if (token->kind == TK_VOID || token->kind == TK_CHAR ||
                   token->kind == TK_SHORT || token->kind == TK_INT ||
                   token->kind == TK_LONG || token->kind == TK_FLOAT ||
                   token->kind == TK_DOUBLE || token->kind == TK_SIGNED ||
                   token->kind == TK_UNSIGNED) {
            if (struct_ != NULL || union_ != NULL || enum_ != NULL) {
                string_t what, info;
                string_from(
                    &what, "cannot combine this with previous type specifiers");
                string_init(&info);
                report(ctx, REPORT_LEVEL_ERROR,
                       (report_info_t){what, info, token->span});
                return false;
            }
            vector_push(tspecs, token);
            tstream_advance(ts);
        } else if (token->kind == TK_CONST || token->kind == TK_RESTRICT ||
                   token->kind == TK_VOLATILE) {
            vector_push(tquals, token);
            tstream_advance(ts);
        } else if (token->kind == TK_INLINE) {
            vector_push(fspecs, token);
            tstream_advance(ts);
        } else if (token->kind == TK_STRUCT) {
            if (tspecs->len != 0 || *union_ != NULL || *enum_ != NULL) {
                string_t what, info;
                string_from(
                    &what, "cannot combine this with previous type specifiers");
                string_init(&info);
                report(ctx, REPORT_LEVEL_ERROR,
                       (report_info_t){what, info, token->span});
                return false;
            }
            *struct_ = type_alloc();
            TRY(parse_struct(ctx, ts, *struct_));
        } else if (token->kind == TK_UNION) {
            if (tspecs->len != 0 || *struct_ != NULL || *enum_ != NULL) {
                string_t what, info;
                string_from(
                    &what, "cannot combine this with previous type specifiers");
                string_init(&info);
                report(ctx, REPORT_LEVEL_ERROR,
                       (report_info_t){what, info, token->span});
                return false;
            }
            *union_ = type_alloc();
            TRY(parse_union(ctx, ts, *union_));
        } else if (token->kind == TK_ENUM) {
            if (tspecs->len != 0 || *struct_ != NULL || *union_ != NULL) {
                string_t what, info;
                string_from(
                    &what, "cannot combine this with previous type specifiers");
                string_init(&info);
                report(ctx, REPORT_LEVEL_ERROR,
                       (report_info_t){what, info, token->span});
                return false;
            }
            *enum_ = type_alloc();
            TRY(parse_enum(ctx, ts, *enum_));
        } else {
            break;
        }
    }
    return true;
}

static bool validate_tquals(context_t *ctx, tstream_t *ts,
                            VECTOR(token_t) tquals, bool *is_const,
                            bool *is_restrict, bool *is_volatile) {
    token_t *token;
    for (size_t i = 0; i < tquals.len; i++) {
        token = vector_at(&tquals, i);
        if (token->kind == TK_CONST) {
            if (*is_const) {
                string_t what, info;
                string_from(&what, "duplicated `const`");
                string_from(&info, "remove this `const`");
                report(ctx, REPORT_LEVEL_WARNING,
                       (report_info_t){what, info, token->span});
            }
            *is_const = true;
        } else if (token->kind == TK_RESTRICT) {
            if (*is_restrict) {
                string_t what, info;
                string_from(&what, "duplicated `restrict`");
                string_from(&info, "remove this `restrict`");
                report(ctx, REPORT_LEVEL_WARNING,
                       (report_info_t){what, info, token->span});
            }
            *is_restrict = true;
        } else if (token->kind == TK_VOLATILE) {
            if (*is_volatile) {
                string_t what, info;
                string_from(&what, "duplicated `volatile`");
                string_from(&info, "remove this `volatile`");
                report(ctx, REPORT_LEVEL_WARNING,
                       (report_info_t){what, info, token->span});
            }
            *is_volatile = true;
        } else {
            panic_internal("unexpected token at validate_tspec");
        }
    }
    return true;
}

static bool validate_tspecs(context_t *ctx, tstream_t *ts,
                            VECTOR(token_t) tspecs, type_kind_t *kind) {
    string_t what, info;
    string_from(&what, "unexpected type");
    string_init(&info);
    span_t span =
        concat_span(((token_t *)vector_at(&tspecs, 0))->span,
                    ((token_t *)vector_at(&tspecs, tspecs.len - 1))->span);

    VECTOR(token_t) sorted_tspecs;
    vector_init(&sorted_tspecs, sizeof(token_t));
    for (size_t i = 0; i < tspecs.len; i++) {
        vector_push(&sorted_tspecs, vector_at(&tspecs, i));
    }
    vector_sort(&sorted_tspecs, tk_cmp);

    tstream_t sorted_tspecs_ts;
    tstream_init(&sorted_tspecs_ts, &sorted_tspecs);
    if (tstream_expect(&sorted_tspecs_ts, TK_VOID, NULL)) {
        if (tstream_eos(&sorted_tspecs_ts)) {
            *kind = TYPE_VOID;
            return true;
        } else {
            goto unexpected;
        }
    } else if (tstream_expect(&sorted_tspecs_ts, TK_CHAR, NULL)) {
        if (tstream_eos(&sorted_tspecs_ts)) {
            *kind = TYPE_CHAR;
            return true;
        } else {
            goto unexpected;
        }
    } else if (tstream_expect(&sorted_tspecs_ts, TK_SHORT, NULL)) {
        if (tstream_eos(&sorted_tspecs_ts)) {
            *kind = TYPE_SHORT;
            return true;
        } else {
            goto unexpected;
        }
    } else if (tstream_expect(&sorted_tspecs_ts, TK_SHORT, NULL)) {
        if (tstream_eos(&sorted_tspecs_ts)) {
            *kind = TYPE_SHORT;
            return true;
        } else {
            goto unexpected;
        }
    } else if (tstream_expect(&sorted_tspecs_ts, TK_INT, NULL)) {
        if (tstream_eos(&sorted_tspecs_ts)) {
            *kind = TYPE_INT;
            return true;
        } else {
            goto unexpected;
        }
    } else if (tstream_expect(&sorted_tspecs_ts, TK_LONG, NULL)) {
        if (tstream_eos(&sorted_tspecs_ts)) {
            *kind = TYPE_LONG;
            return true;
        } else {
            if (tstream_expect(&sorted_tspecs_ts, TK_LONG, NULL)) {
                *kind = TYPE_LONGLONG;
                return true;
            } else if (tstream_expect(&sorted_tspecs_ts, TK_DOUBLE, NULL)) {
                *kind = TYPE_LONGDOUBLE;
                return true;
            } else {
                goto unexpected;
            }
        }
    } else if (tstream_expect(&sorted_tspecs_ts, TK_FLOAT, NULL)) {
        if (tstream_eos(&sorted_tspecs_ts)) {
            *kind = TYPE_FLOAT;
            return true;
        } else {
            goto unexpected;
        }
    } else if (tstream_expect(&sorted_tspecs_ts, TK_DOUBLE, NULL)) {
        if (tstream_eos(&sorted_tspecs_ts)) {
            *kind = TYPE_DOUBLE;
            return true;
        } else {
            goto unexpected;
        }
    } else if (tstream_expect(&sorted_tspecs_ts, TK_UNSIGNED, NULL)) {
    } else if (tstream_expect(&sorted_tspecs_ts, TK_SIGNED, NULL)) {
    }

unexpected:
    report(ctx, REPORT_LEVEL_ERROR, (report_info_t){what, info, span});
    return false;
}

static bool validate_type(context_t *ctx, tstream_t *ts, VECTOR(token_t) tspecs,
                          VECTOR(token_t) tquals, type_t *struct_,
                          type_t *union_, type_t *enum_, type_t *type) {
    TRY(validate_tquals(ctx, ts, tquals, &type->is_const, &type->is_restrict,
                        &type->is_volatile));

    if (tspecs.len != 0) {
        TRY(validate_tspecs(ctx, ts, tspecs, &type->kind));
    } else if (struct_ != NULL) {
        type->kind = TYPE_STRUCT;
        type->struct_ = struct_->struct_;
        type->span = struct_->span;
    } else if (union_ != NULL) {
        type->kind = TYPE_UNION;
        type->union_ = union_->union_;
        type->span = union_->span;
    } else if (enum_ != NULL) {
        type->kind = TYPE_ENUM;
        type->enum_ = enum_->enum_;
        type->span = enum_->span;
    } else {
        panic_internal("unexpected argument at validate_type");
    }

    return true;
}

bool parse_decls(context_t *ctx, tstream_t *ts, VECTOR_REF(decl_t *) decls) {
    type_t *struct_, *union_, *enum_;
    VECTOR(token_t) sspecs, tspecs, tquals, fspecs;
    TRY(collect_type_infos(ctx, ts, &struct_, &union_, &enum_, &sspecs, &tspecs,
                           &tquals, &fspecs));

    type_t *type = type_alloc();
    TRY(validate_type(ctx, ts, tspecs, tquals, struct_, union_, enum_, type));

    vector_init(decls, sizeof(decl_t *));
    return true;
}
