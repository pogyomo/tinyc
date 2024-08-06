#include "type.h"

#include <assert.h>
#include <stdlib.h>

#include "../report.h"
#include "ast/decl.h"
#include "ast/type.h"
#include "context.h"
#include "expr.h"
#include "stream.h"

bool parse_type_name(struct parse_context *ctx, struct tstream *ts,
                     struct type **type) {
    TRY(parse_type_head(ctx, ts, type, NULL, NULL));
    TRY(parse_type_rest(ctx, ts, *type, type, NULL));
    return true;
}

bool ts_startwith_type(struct parse_context *ctx, struct tstream *ts) {
    struct tstream ts_save = *ts;
    context_suppress_report(ctx->ctx);

    struct type *type;
    struct storage_class *class;
    struct function_spec *func_spec;
    bool res = parse_type_head(ctx, ts, &type, &class, &func_spec);

    context_activate_report(ctx->ctx);
    *ts = ts_save;
    return res;
}

// Parse a field by assuming its base type is `head`.
static bool parse_struct_or_union_field(
    struct parse_context *ctx, struct tstream *ts, struct type *head,
    struct type_struct_or_union_decl **decl) {
    struct type *rest;
    struct type_rest_name name;
    TRY(parse_type_rest(ctx, ts, head, &rest, &name));

    if (!name.exists) {
        struct report_info info = {REPORT_ERROR, rest->span,
                                   "field name expected", ""};
        report(ctx->ctx, &info);
        return false;
    }

    struct span span = rest->span;
    struct expr *bit_field = NULL;
    if (tstream_is_punct(ts, TK_PUNCT_COLON)) {
        tstream_advance(ts);
        TRY(parse_expr(ctx, ts, &bit_field));
        merge_span(&span, &bit_field->span, &span);
    }

    struct type_struct_or_union_decl_name name_ = {name.name, name.span};
    *decl = type_struct_or_union_decl_new(rest, &name_, &span, bit_field);
    return true;
}

// Parse struct name { ... } or union name { ... }.
static bool parse_struct_or_union_type(struct parse_context *ctx,
                                       struct tstream *ts, struct type **type) {
    struct span span;

    bool is_struct = tstream_is_keyword(ts, TK_KEYWORD_STRUCT);
    span = tstream_curr(ts)->span;
    tstream_advance(ts);

    struct type_struct_or_union_name name = {false};
    if (!tstream_is_punct(ts, TK_PUNCT_LCURLY)) {
        TRY(tstream_check_ident(ctx->ctx, ts));
        name.exists = true;
        name.span = tstream_curr(ts)->span;
        name.value = tstream_curr(ts)->ident.value.str;
        merge_span(&span, &name.span, &span);
        tstream_advance(ts);
    }

    if (!tstream_is_punct(ts, TK_PUNCT_LCURLY) && !name.exists) {
        struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                   "expect `{` after this", ""};
        report(ctx->ctx, &info);
        return false;
    }
    tstream_advance(ts);

    struct type_struct_or_union_decl decl_head = {NULL};
    struct type_struct_or_union_decl *decl_prev = &decl_head;
    while (true) {
        if (tstream_is_punct(ts, TK_PUNCT_RCURLY)) {
            break;
        } else {
            while (true) {
                struct type_struct_or_union_decl *decl;
                struct type *head;
                TRY(parse_type_head(ctx, ts, &head, NULL, NULL));
                TRY(parse_struct_or_union_field(ctx, ts, head, &decl));
                decl_prev->next = decl;
                decl_prev = decl_prev->next;

                if (tstream_is_punct(ts, TK_PUNCT_COMMA)) {
                    tstream_advance(ts);
                } else if (tstream_is_punct(ts, TK_PUNCT_SEMICOLON)) {
                    tstream_advance(ts);
                    break;
                }
            }
        }
    }
    merge_span(&span, &tstream_curr(ts)->span, &span);
    tstream_advance(ts);

    *type =
        type_struct_or_union_new(is_struct, NULL, &name, decl_head.next, &span);

    return true;
}

static int comp_token_keyword_kind(const void *a, const void *b) {
    return *(enum token_keyword_kind *)a - *(enum token_keyword_kind *)b;
}

// Parse builtin type like `int`, `unsigned long long`, etc.
static bool parse_builtin_type(struct parse_context *ctx, struct tstream *ts,
                               struct type **type) {
    static enum token_keyword_kind keywords[] = {
        TK_KEYWORD_VOID,   TK_KEYWORD_CHAR,     TK_KEYWORD_SHORT,
        TK_KEYWORD_INT,    TK_KEYWORD_LONG,     TK_KEYWORD_FLOAT,
        TK_KEYWORD_DOUBLE, TK_KEYWORD_UNSIGNED, TK_KEYWORD_SIGNED,
        TK_KEYWORD__BOOL,  TK_KEYWORD__COMPLEX, TK_KEYWORD__IMAGINARY,
    };
    static struct {
        enum token_keyword_kind keywords[4];
        size_t num_keyword;
        enum type_builtin_kind type_kind;
    } kw_to_type[] = {
        // void
        {{TK_KEYWORD_VOID}, 1, TYPE_BUILTIN_VOID},
        // char
        {{TK_KEYWORD_CHAR}, 1, TYPE_BUILTIN_CHAR},
        {{TK_KEYWORD_SIGNED, TK_KEYWORD_CHAR}, 2, TYPE_BUILTIN_CHAR},
        // unsigned char
        {{TK_KEYWORD_UNSIGNED, TK_KEYWORD_CHAR}, 2, TYPE_BUILTIN_UCHAR},
        // short
        {{TK_KEYWORD_SHORT}, 1, TYPE_BUILTIN_SHORT},
        {{TK_KEYWORD_SIGNED, TK_KEYWORD_SHORT}, 2, TYPE_BUILTIN_SHORT},
        {{TK_KEYWORD_SHORT, TK_KEYWORD_INT}, 2, TYPE_BUILTIN_SHORT},
        {{TK_KEYWORD_SIGNED, TK_KEYWORD_SHORT, TK_KEYWORD_INT},
         3,
         TYPE_BUILTIN_SHORT},
        // unsigned short
        {{TK_KEYWORD_UNSIGNED, TK_KEYWORD_SHORT}, 2, TYPE_BUILTIN_USHORT},
        {{TK_KEYWORD_UNSIGNED, TK_KEYWORD_SHORT, TK_KEYWORD_INT},
         3,
         TYPE_BUILTIN_USHORT},
        // int
        {{TK_KEYWORD_INT}, 1, TYPE_BUILTIN_INT},
        {{TK_KEYWORD_SIGNED}, 1, TYPE_BUILTIN_INT},
        {{TK_KEYWORD_SIGNED, TK_KEYWORD_INT}, 2, TYPE_BUILTIN_INT},
        // unsigned int
        {{TK_KEYWORD_UNSIGNED}, 1, TYPE_BUILTIN_UINT},
        {{TK_KEYWORD_UNSIGNED, TK_KEYWORD_INT}, 2, TYPE_BUILTIN_UINT},
        // long
        {{TK_KEYWORD_LONG}, 1, TYPE_BUILTIN_LONG},
        {{TK_KEYWORD_SIGNED, TK_KEYWORD_LONG}, 2, TYPE_BUILTIN_LONG},
        {{TK_KEYWORD_LONG, TK_KEYWORD_INT}, 2, TYPE_BUILTIN_LONG},
        {{TK_KEYWORD_SIGNED, TK_KEYWORD_LONG, TK_KEYWORD_INT},
         3,
         TYPE_BUILTIN_LONG},
        // unsigned long
        {{TK_KEYWORD_UNSIGNED, TK_KEYWORD_LONG}, 2, TYPE_BUILTIN_ULONG},
        {{TK_KEYWORD_UNSIGNED, TK_KEYWORD_LONG, TK_KEYWORD_INT},
         3,
         TYPE_BUILTIN_ULONG},
        // long long
        {{TK_KEYWORD_LONG, TK_KEYWORD_LONG}, 2, TYPE_BUILTIN_LLONG},
        {{TK_KEYWORD_SIGNED, TK_KEYWORD_LONG, TK_KEYWORD_LONG},
         3,
         TYPE_BUILTIN_LLONG},
        {{TK_KEYWORD_LONG, TK_KEYWORD_LONG, TK_KEYWORD_INT},
         3,
         TYPE_BUILTIN_LLONG},
        {{TK_KEYWORD_SIGNED, TK_KEYWORD_LONG, TK_KEYWORD_LONG, TK_KEYWORD_INT},
         4,
         TYPE_BUILTIN_LLONG},
        // unsigned long long
        {{TK_KEYWORD_UNSIGNED, TK_KEYWORD_LONG, TK_KEYWORD_LONG},
         3,
         TYPE_BUILTIN_ULLONG},
        {{TK_KEYWORD_UNSIGNED, TK_KEYWORD_LONG, TK_KEYWORD_LONG,
          TK_KEYWORD_INT},
         4,
         TYPE_BUILTIN_ULLONG},
        // float
        {{TK_KEYWORD_FLOAT}, 1, TYPE_BUILTIN_FLOAT},
        // double
        {{TK_KEYWORD_DOUBLE}, 1, TYPE_BUILTIN_DOUBLE},
        // long double
        {{TK_KEYWORD_LONG, TK_KEYWORD_DOUBLE}, 2, TYPE_BUILTIN_LDOUBLE},
        // _Bool
        {{TK_KEYWORD__BOOL}, 1, TYPE_BUILTIN_BOOL},
        // float _Complex
        {{TK_KEYWORD_FLOAT, TK_KEYWORD__COMPLEX}, 2, TYPE_BUILTIN_C_FLOAT},
        // double _Complex
        {{TK_KEYWORD_DOUBLE, TK_KEYWORD__COMPLEX}, 2, TYPE_BUILTIN_C_DOUBLE},
        // long double _Complex
        {{TK_KEYWORD_LONG, TK_KEYWORD_DOUBLE, TK_KEYWORD__COMPLEX},
         3,
         TYPE_BUILTIN_C_LDOUBLE},
        // float _Imaginary
        {{TK_KEYWORD_FLOAT, TK_KEYWORD__IMAGINARY}, 2, TYPE_BUILTIN_I_FLOAT},
        // double _Imaginary
        {{TK_KEYWORD_DOUBLE, TK_KEYWORD__IMAGINARY}, 2, TYPE_BUILTIN_I_DOUBLE},
        // long double _Imaginary
        {{TK_KEYWORD_LONG, TK_KEYWORD_DOUBLE, TK_KEYWORD__IMAGINARY},
         3,
         TYPE_BUILTIN_I_LDOUBLE},
    };

    struct span span;
    size_t len = 0;
    enum token_keyword_kind extracted[5];
    for (size_t i = 0; i < sizeof extracted / sizeof extracted[0]; i++) {
        bool found = false;
        for (size_t j = 0; j < sizeof keywords / sizeof keywords[0]; j++) {
            if (tstream_is_keyword(ts, keywords[j])) {
                if (len == 0)
                    span = tstream_curr(ts)->span;
                else
                    merge_span(&span, &tstream_curr(ts)->span, &span);

                tstream_advance(ts);

                extracted[i] = keywords[j];
                found = true;
                len++;
                break;
            }
        }
        if (!found) break;
    }

    if (len == 0) {
        struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                   "expected type specifier after this", ""};
        report(ctx->ctx, &info);
        return false;
    } else if (len == 5) {
        struct report_info info = {REPORT_ERROR, span,
                                   "too many type specifier", ""};
        report(ctx->ctx, &info);
        return false;
    }

    // Normalize the order of keywords.
    qsort(extracted, len, sizeof(enum token_keyword_kind),
          &comp_token_keyword_kind);

    for (size_t i = 0; i < sizeof kw_to_type / sizeof kw_to_type[0]; i++) {
        if (kw_to_type[i].num_keyword != len) continue;
        for (size_t j = 0; j < len; j++) {
            if (kw_to_type[i].keywords[j] != extracted[j]) continue;
        }
        *type = type_builtin_new(kw_to_type[i].type_kind, NULL, &span);
        return true;
    }
    struct report_info info = {REPORT_ERROR, span,
                               "invalid combination of type specifier", ""};
    report(ctx->ctx, &info);
    return false;
}

static bool extract_type_qual(struct parse_context *ctx, struct tstream *ts,
                              struct type_qual **qual) {
    static struct {
        enum token_keyword_kind kw_kind;
        enum type_qual_kind qual_kind;
    } kw_to_qual[] = {
        {TK_KEYWORD_CONST, TYPE_QUAL_CONST},
        {TK_KEYWORD_RESTRICT, TYPE_QUAL_RESTRICT},
        {TK_KEYWORD_VOLATILE, TYPE_QUAL_VOLATILE},
    };
    for (size_t i = 0; i < sizeof kw_to_qual / sizeof kw_to_qual[0]; i++) {
        if (tstream_is_keyword(ts, kw_to_qual[i].kw_kind)) {
            *qual =
                type_qual_new(kw_to_qual[i].qual_kind, &tstream_curr(ts)->span);
            tstream_advance(ts);
            return true;
        }
    }
    return false;
}

bool parse_type_head(struct parse_context *ctx, struct tstream *ts,
                     struct type **type, struct storage_class **class,
                     struct function_spec **func_spec) {
    static struct {
        enum token_keyword_kind kw_kind;
        enum storage_class_kind class_kind;
    } kw_to_class[] = {
        {TK_KEYWORD_TYPEDEF, STORAGE_CLASS_TYPEDEF},
        {TK_KEYWORD_EXTERN, STORAGE_CLASS_EXTERN},
        {TK_KEYWORD_STATIC, STORAGE_CLASS_STATIC},
        {TK_KEYWORD_AUTO, STORAGE_CLASS_AUTO},
        {TK_KEYWORD_REGISTER, STORAGE_CLASS_REGISTER},
    };

    if (class) *class = NULL;
    if (func_spec) *func_spec = NULL;

    bool type_found = false;
    struct type_qual head = {NULL};
    struct type_qual *prev = &head;
    while (true) {
        bool class_found = false;
        for (size_t i = 0; i < sizeof kw_to_class / sizeof kw_to_class[0];
             i++) {
            if (tstream_is_keyword(ts, kw_to_class[i].kw_kind)) {
                if (!class) {
                    struct report_info info = {
                        REPORT_ERROR, tstream_curr(ts)->span,
                        "storage class specifier is not expected here", ""};
                    report(ctx->ctx, &info);
                    return false;
                } else if (*class) {
                    struct report_info info = {REPORT_ERROR,
                                               tstream_curr(ts)->span,
                                               "cannot use this with other "
                                               "storage class specifier",
                                               ""};
                    report(ctx->ctx, &info);
                    return false;
                } else {
                    *class = storage_class_new(kw_to_class[i].class_kind,
                                               &tstream_curr(ts)->span);
                    tstream_advance(ts);
                    class_found = true;
                    break;
                }
            }
        }
        if (class_found) continue;

        if (extract_type_qual(ctx, ts, &prev->next)) {
            prev = prev->next;
            continue;
        }

        if (tstream_is_keyword(ts, TK_KEYWORD_INLINE)) {
            if (!func_spec) {
                struct report_info info = {
                    REPORT_ERROR, tstream_curr(ts)->span,
                    "function specifier is not expected here", ""};
                report(ctx->ctx, &info);
                return false;
            } else if (*func_spec) {
                struct report_info info = {
                    REPORT_ERROR, tstream_curr(ts)->span,
                    "cannot use this with other function specifier", ""};
                report(ctx->ctx, &info);
                return false;
            } else {
                *func_spec = function_spec_new(FUNCTION_SPEC_INLINE,
                                               &tstream_curr(ts)->span);
                tstream_advance(ts);
                continue;
            }
        }

        if (type_found) break;
        if (tstream_is_ident(ts)) {
            char *value = tstream_curr(ts)->ident.value.str;
            if (parse_context_typedef_name_exists(ctx, value)) {
                *type = type_typedef_name_new(value, &tstream_curr(ts)->span);
                type_found = true;
                tstream_advance(ts);
                continue;
            } else {
                struct report_info info = {REPORT_ERROR, tstream_curr(ts)->span,
                                           "no such typedef name exists", ""};
                report(ctx->ctx, &info);
                return false;
            }
        } else if (tstream_is_keyword(ts, TK_KEYWORD_STRUCT) ||
                   tstream_is_keyword(ts, TK_KEYWORD_UNION)) {
            TRY(parse_struct_or_union_type(ctx, ts, type));
            type_found = true;
        } else {
            TRY(parse_builtin_type(ctx, ts, type));
            type_found = true;
        }
    }
    (*type)->quals = head.next;
    return true;
}

// Parse `[...]` in type, merge it into `*type`.
// `type` and `*type` must not be NULL.
static bool parse_array(struct parse_context *ctx, struct tstream *ts,
                        struct type **type) {
    assert(type != NULL && *type != NULL);

    bool has_static = false;
    struct span span = (*type)->span;

    assert(tstream_is_punct(ts, TK_PUNCT_LSQUARE));
    tstream_advance(ts);

    if (tstream_is_keyword(ts, TK_KEYWORD_STATIC)) {
        has_static = true;
        tstream_advance(ts);
    }

    struct type_qual quals_head = {NULL};
    struct type_qual *quals_prev = &quals_head;
    while (extract_type_qual(ctx, ts, &quals_prev->next)) {
        quals_prev = quals_prev->next;
    }

    if (!has_static && tstream_is_keyword(ts, TK_KEYWORD_STATIC)) {
        has_static = true;
        tstream_advance(ts);
    }

    struct type_array_size *size = NULL;
    if (tstream_is_punct(ts, TK_PUNCT_STAR)) {
        if (has_static) {
            struct report_info info = {
                REPORT_ERROR, tstream_last(ts)->span,
                "`*` doesn't expected to static modified array", ""};
            report(ctx->ctx, &info);
            return false;
        } else {
            size = type_array_size_star_new(&tstream_curr(ts)->span);
            tstream_advance(ts);
        }
    } else {
        if (!tstream_is_punct(ts, TK_PUNCT_RSQUARE)) {
            struct expr *value;
            TRY(parse_assign_expr(ctx, ts, &value));
            size = type_array_size_expr_new(value, &value->span);
        }
    }

    TRY(tstream_check_punct(ctx->ctx, ts, TK_PUNCT_RSQUARE));
    merge_span(&span, &tstream_curr(ts)->span, &span);
    tstream_advance(ts);

    *type = type_array_new(*type, has_static, quals_head.next, size, &span);
    return true;
}

// Parse `(...)` in type, merge it into `*type`.
// `type` and `*type` must not be NULL.
static bool parse_func(struct parse_context *ctx, struct tstream *ts,
                       struct type **type) {
    assert(type != NULL && *type != NULL);

    struct span span = (*type)->span;

    assert(tstream_is_punct(ts, TK_PUNCT_LPAREN));
    tstream_advance(ts);

    struct type_func_param param_head = {NULL};
    struct type_func_param *param_prev = &param_head;
    bool is_first = true;
    while (true) {
        if (is_first && tstream_is_punct(ts, TK_PUNCT_RPAREN)) {
            break;
        } else if (tstream_is_punct(ts, TK_PUNCT_DOTDOTDOT)) {
            param_prev->next =
                type_func_param_varidic_new(&tstream_curr(ts)->span);
            param_prev = param_prev->next;

            if (tstream_is_punct(ts, TK_PUNCT_RPAREN)) {
                break;
            } else {
                struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                           "expected `)` after this", ""};
                report(ctx->ctx, &info);
                return false;
            }
        } else {
            struct type *head, *type;
            struct type_rest_name name_;
            TRY(parse_type_head(ctx, ts, &head, NULL, NULL));
            TRY(parse_type_rest(ctx, ts, head, &type, &name_));

            struct type_func_param_name name = {name_.exists, name_.name,
                                                name_.span};
            struct span span = type->span;
            if (name.exists) merge_span(&span, &name.span, &span);
            param_prev->next = type_func_param_normal_new(type, &name, &span);
            param_prev = param_prev->next;

            if (tstream_is_punct(ts, TK_PUNCT_RPAREN)) {
                break;
            } else if (tstream_is_punct(ts, TK_PUNCT_COMMA)) {
                tstream_advance(ts);
            } else {
                struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                           "expected `)` or `,` after this",
                                           ""};
                report(ctx->ctx, &info);
                return false;
            }
        }
        is_first = false;
    }

    merge_span(&span, &tstream_curr(ts)->span, &span);
    tstream_advance(ts);

    *type = type_func_new(*type, param_head.next, &span);
    return true;
}

bool parse_type_rest(struct parse_context *ctx, struct tstream *ts,
                     struct type *head, struct type **type,
                     struct type_rest_name *name) {
    while (true) {
        if (tstream_is_punct(ts, TK_PUNCT_STAR)) {
            struct span span = head->span;
            tstream_advance(ts);

            struct type_qual quals_head = {NULL};
            struct type_qual *quals_prev = &quals_head;
            while (extract_type_qual(ctx, ts, &quals_prev->next)) {
                quals_prev = quals_prev->next;
            }

            merge_span(&span, &tstream_last(ts)->span, &span);

            head = type_pointer_new(head, quals_head.next, &span);
        } else {
            break;
        }
    }

    // Extract name, or just skip `(...)`.
    struct tstream *inner_ts = NULL;  // tstraem start with inner of `(...)`.
    if (tstream_is_punct(ts, TK_PUNCT_LPAREN)) {
        tstream_advance(ts);
        inner_ts = ts;

        uint64_t depth = 1;
        while (!tstream_eos(ts)) {
            if (tstream_is_punct(ts, TK_PUNCT_RPAREN)) {
                if (--depth == 0) break;
            } else if (tstream_is_punct(ts, TK_PUNCT_LPAREN)) {
                depth++;
            } else {
                tstream_advance(ts);
            }
        }
        if (depth != 0) {
            struct report_info info = {REPORT_ERROR, tstream_last(ts)->span,
                                       "expected `)` after this", ""};
            report(ctx->ctx, &info);
            return false;
        }
    } else if (tstream_is_ident(ts)) {
        if (!name) {
            struct report_info info = {REPORT_ERROR, tstream_curr(ts)->span,
                                       "identifier is not expected here", ""};
            report(ctx->ctx, &info);
            return false;
        } else if (parse_context_typedef_name_exists(
                       ctx, tstream_curr(ts)->ident.value.str)) {
            struct report_info info = {
                REPORT_ERROR, tstream_curr(ts)->span,
                "expected variable name, found type name", ""};
            report(ctx->ctx, &info);
            return false;
        }
        name->exists = true;
        name->name = tstream_curr(ts)->ident.value.str;
        name->span = tstream_curr(ts)->span;
        tstream_advance(ts);
    }

    while (true) {
        if (tstream_is_punct(ts, TK_PUNCT_LSQUARE)) {
            TRY(parse_array(ctx, ts, &head));
        } else if (tstream_is_punct(ts, TK_PUNCT_LPAREN)) {
            TRY(parse_func(ctx, ts, &head));
        } else {
            break;
        }
    }

    if (inner_ts)
        TRY(parse_type_rest(ctx, inner_ts, head, type, name));
    else
        *type = head;

    return true;
}
