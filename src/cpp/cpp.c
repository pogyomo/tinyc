#include "cpp.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "../context.h"
#include "../report.h"
#include "context.h"
#include "stream.h"
#include "token.h"

static struct {
    char *name;
    enum token_punct_kind kind;
} puncts[] = {
    {"||", TK_PUNCT_OR},
    {"|=", TK_PUNCT_ORASSIGN},
    {"|", TK_PUNCT_VERTICAL},
    {"^=", TK_PUNCT_XORASSIGN},
    {"^", TK_PUNCT_HAT},
    {"&&", TK_PUNCT_AND},
    {"&=", TK_PUNCT_ANDASSIGN},
    {"&", TK_PUNCT_AMPERSAND},
    {"<<=", TK_PUNCT_LSHIFTASSIGN},
    {"<<", TK_PUNCT_LSHIFT},
    {"<=", TK_PUNCT_LE},
    {"<", TK_PUNCT_LT},
    {">>=", TK_PUNCT_RSHIFTASSIGN},
    {">>", TK_PUNCT_RSHIFT},
    {">=", TK_PUNCT_GE},
    {">", TK_PUNCT_GT},
    {"+=", TK_PUNCT_ADDASSIGN},
    {"++", TK_PUNCT_PLUSPLUS},
    {"+", TK_PUNCT_PLUS},
    {"-=", TK_PUNCT_SUBASSIGN},
    {"--", TK_PUNCT_MINUSMINUS},
    {"-", TK_PUNCT_MINUS},
    {"*=", TK_PUNCT_MULASSIGN},
    {"*", TK_PUNCT_STAR},
    {"/=", TK_PUNCT_DIVASSIGN},
    {"/", TK_PUNCT_SLASH},
    {"%=", TK_PUNCT_MODASSIGN},
    {"%", TK_PUNCT_PERCENT},
    {"(", TK_PUNCT_LPAREN},
    {")", TK_PUNCT_RPAREN},
    {"{", TK_PUNCT_LCURLY},
    {"}", TK_PUNCT_RCURLY},
    {"[", TK_PUNCT_LSQUARE},
    {"]", TK_PUNCT_RSQUARE},
    {";", TK_PUNCT_SEMICOLON},
    {":", TK_PUNCT_COLON},
    {",", TK_PUNCT_COMMA},
    {"~", TK_PUNCT_TILDE},
    {"!=", TK_PUNCT_NE},
    {"!", TK_PUNCT_NOT},
    {"?", TK_PUNCT_QUESTION},
    {"#", TK_PUNCT_SHARP},
    {".", TK_PUNCT_DOT},
    {"->", TK_PUNCT_ARROW},
    {"==", TK_PUNCT_EQ},
    {"=", TK_PUNCT_ASSIGN},
};

static bool is_ident_head(char c) { return isalpha(c) || c == '_'; }

static bool is_ident_rest(char c) { return isalnum(c) || c == '_'; }

// Construct list of `cpp_token` from `s`, then store it to `out_tokens`.
// This function returns false if and only if the construction failed.
// This correspond to from 1.2 to 1.3 in 5.1.1.2.
bool tokenize(struct cpp_context *ctx, const char *s,
              struct cpp_token **out_tokens) {
    struct stream st;
    stream_init(&st, s);

    struct cpp_token head = {NULL};
    struct cpp_token *prev = &head;

    while (!stream_eos(&st)) {
        struct position start = {st.row, st.col};
        struct position end = start;

        bool found = false;
        for (size_t i = 0; i < sizeof puncts / sizeof puncts[0]; i++) {
            if (puncts[i].kind == TK_PUNCT_DOT)
                continue;  // pp-number may use this.

            if (stream_accept_s(&st, puncts[i].name, &end)) {
                struct span span = {ctx->id, start, end};
                prev->next = cpp_token_new(CPP_TK_PUNCT, &span);
                prev->next->punct.kind = puncts[i].kind;
                prev = prev->next;
                found = true;
                break;
            }
        }
        if (found) continue;

        if (stream_accept_c(&st, ' ', &end)) {
            struct span span = {ctx->id, start, end};
            prev->next = cpp_token_new(CPP_TK_SPACE, &span);
            prev = prev->next;
        } else if (stream_accept_c(&st, '\n', &end)) {
            struct span span = {ctx->id, start, end};
            prev->next = cpp_token_new(CPP_TK_NEWLINE, &span);
            prev = prev->next;
        } else if (stream_accept_s(&st, "//", &end)) {
            while (stream_eos(&st)) {
                if (stream_char(&st) == '\n') {
                    break;
                } else {
                    stream_advance(&st, &end);
                }
            }
            struct span span = {ctx->id, start, end};
            prev->next = cpp_token_new(CPP_TK_SPACE, &span);
            prev = prev->next;
        } else if (stream_accept_s(&st, "/*", &end)) {
            while (true) {
                if (stream_eos(&st)) {
                    struct report_info info = {REPORT_ERROR,
                                               {ctx->id, start, end},
                                               "unclosing comment",
                                               ""};
                    report(ctx->ctx, &info);
                    return NULL;
                } else if (stream_accept_s(&st, "*/", &end)) {
                    break;
                } else {
                    stream_advance(&st, &end);
                }
            }
            struct span span = {ctx->id, start, end};
            prev->next = cpp_token_new(CPP_TK_SPACE, &span);
            prev = prev->next;
        } else if (is_ident_head(stream_char(&st))) {
            struct string buf;
            string_init(&buf);
            while (!stream_eos(&st)) {
                if (is_ident_rest(stream_char(&st))) {
                    string_push(&buf, stream_char(&st));
                    stream_advance(&st, &end);
                } else {
                    break;
                }
            }

            struct span span = {ctx->id, start, end};
            prev->next = cpp_token_new(CPP_TK_IDENT, &span);
            prev->next->ident.value = buf;
            prev = prev->next;
        } else if (isdigit(stream_char(&st)) || stream_char(&st) == '.') {
            struct string buf;
            string_init(&buf);

            if (stream_accept_c(&st, '.', &end)) {
                string_push(&buf, '.');
                if (stream_eos(&st) || !isdigit(stream_char(&st))) {
                    struct span span = {ctx->id, start, end};
                    prev->next = cpp_token_new(CPP_TK_PUNCT, &span);
                    prev->pp_number.value = buf;
                    prev = prev->next;
                    continue;
                }
            }

            char *exps[] = {"e+", "e-", "E+", "E-", "p+", "p-", "P+", "P-"};
            while (!stream_eos(&st)) {
                bool exp_found = false;
                for (size_t i = 0; i < sizeof exps / sizeof exps[0]; i++) {
                    if (stream_accept_s(&st, exps[i], &end)) {
                        string_append(&buf, exps[i]);
                        exp_found = true;
                        break;
                    }
                }
                if (exp_found) continue;

                if (isalnum(stream_char(&st))) {
                    string_push(&buf, stream_char(&st));
                    stream_advance(&st, &end);
                } else if (stream_accept_c(&st, '.', &end)) {
                    string_push(&buf, '.');
                } else {
                    break;
                }
            }

            struct span span = {ctx->id, start, end};
            prev->next = cpp_token_new(CPP_TK_PP_NUMBER, &span);
            prev->next->pp_number.value = buf;
            prev = prev->next;
        } else if (stream_accept_c(&st, '\'', &end)) {
            // TODO: imcomplete
            struct string buf;
            string_init(&buf);
            while (!stream_eos(&st)) {
                if (stream_char(&st) == '\n') {
                    struct report_info info = {REPORT_ERROR,
                                               {ctx->id, start, end},
                                               "unclosing character constant",
                                               ""};
                    report(ctx->ctx, &info);
                    return NULL;
                } else if (stream_accept_c(&st, '\'', &end)) {
                    break;
                } else {
                    stream_advance(&st, &end);
                }
            }
            struct span span = {ctx->id, start, end};
            prev->next = cpp_token_new(CPP_TK_CHAR, &span);
            prev->next->char_.value = buf;
            prev = prev->next;
        } else if (stream_accept_c(&st, '"', &end)) {
            // TODO: imcomplete
            struct string buf;
            string_init(&buf);
            while (!stream_eos(&st)) {
                if (stream_char(&st) == '\n') {
                    struct report_info info = {REPORT_ERROR,
                                               {ctx->id, start, end},
                                               "unclosing character constant",
                                               ""};
                    report(ctx->ctx, &info);
                    return NULL;
                } else if (stream_accept_c(&st, '"', &end)) {
                    break;
                } else {
                    stream_advance(&st, &end);
                }
            }
            struct span span = {ctx->id, start, end};
            prev->next = cpp_token_new(CPP_TK_STRING, &span);
            prev->next->string.value = buf;
            prev = prev->next;
        } else {
            stream_advance(&st, &end);
            struct span span = {ctx->id, start, end};
            prev->next = cpp_token_new(CPP_TK_UNKNOWN, &span);
            prev = prev->next;
        }
    }

    *out_tokens = head.next;
    return true;
}

// Expand macro in `in_tokens` until newline, then store it to `out_tokens`.
bool expand(struct cpp_context *ctx, struct cpp_token *in_tokens,
            struct cpp_tokens **out_tokens) {}

// Execute preprocessing directive and do macro invocation for given
// `in_tokens`, then store it to `out_tokens`. Returns false if and only if the
// convertion failed.
// This correspond to 1.4 in 5.1.1.2.
bool preprocess(struct cpp_context *ctx, struct cpp_token *in_tokens,
                struct cpp_token **out_tokens) {
    // struct cpp_token *in_curr = in_tokens->head;
    // struct cpp_token out_head;
    // struct cpp_token *out_curr = &out_head;
    // while (in_curr) {
    //     if (in_curr->kind != CPP_TK_PUNCT || in_curr->punct !=
    //     TK_PUNCT_SHARP) {
    //     }
    // }

    // TODO: Implement
    *out_tokens = in_tokens;

    return true;
}

bool cpp_file(struct context *ctx, const char *path,
              struct cpp_token **tokens) {
    size_t id = context_cache_file(ctx, path);
    const struct cache_entry *entry = context_fetch(ctx, id);

    struct cpp_context cpp_ctx;
    cpp_context_init(&cpp_ctx, ctx, id);

    struct cpp_token *pre_tokens;
    if (!tokenize(&cpp_ctx, entry->content.str, &pre_tokens)) return false;
    if (!preprocess(&cpp_ctx, pre_tokens, tokens)) return false;

    return true;
}
