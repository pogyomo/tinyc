#include "tinyc/cpp/params.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "tinyc/cpp/context.h"
#include "tinyc/repo.h"
#include "tinyc/source.h"
#include "tinyc/span.h"
#include "tinyc/token.h"

static inline tinyc_repo_id dummy_repo(struct tinyc_repo *repo) {
    struct tinyc_source source;
    tinyc_source_from_str(&source, "name", "line1\nline2\nline3\n");
    tinyc_repo_init(repo);
    return tinyc_repo_registory(repo, &source);
}

static inline struct tinyc_token *create_lparen(
    const struct tinyc_span *span,
    int tspaces
) {
    return tinyc_token_create_punct(span, 0, TINYC_TOKEN_PUNCT_LPAREN);
}

static inline struct tinyc_token *create_rparen(
    const struct tinyc_span *span,
    int tspaces
) {
    return tinyc_token_create_punct(span, 0, TINYC_TOKEN_PUNCT_RPAREN);
}

static inline struct tinyc_token *create_ident(
    const struct tinyc_span *span,
    int tspaces,
    const char *value
) {
    return tinyc_token_create_ident(span, 0, value);
}

static inline struct tinyc_token *create_comma(
    const struct tinyc_span *span,
    int tspaces
) {
    return tinyc_token_create_punct(span, tspaces, TINYC_TOKEN_PUNCT_COMMA);
}

static inline struct tinyc_token *create_plus(
    const struct tinyc_span *span,
    int tspaces
) {
    return tinyc_token_create_punct(span, tspaces, TINYC_TOKEN_PUNCT_PLUS);
}

static inline void no_param(void) {
    struct tinyc_repo repo;
    tinyc_repo_id id = dummy_repo(&repo);
    struct tinyc_span span = {
        id,
        {0, 0},
        {0, 0}
    };

    struct tinyc_token *lparen = create_lparen(&span, 0);
    struct tinyc_token *rparen = create_rparen(&span, 0);
    struct tinyc_token *tokens = lparen, *head = tokens;
    tokens = tinyc_token_insert(tokens, rparen);

    struct tinyc_cpp_context ctx;
    tinyc_cpp_context_init(&ctx);

    struct tinyc_token *it = head;
    struct tinyc_cpp_macro_func_param *args;
    assert(tinyc_cpp_parse_params(&ctx, &repo, head, &it, &args));
    assert(tinyc_token_is_punct_of(it, TINYC_TOKEN_PUNCT_RPAREN));

    assert(args == NULL);
}

static inline void one_param(void) {
    struct tinyc_repo repo;
    tinyc_repo_id id = dummy_repo(&repo);
    struct tinyc_span span = {
        id,
        {0, 0},
        {0, 0}
    };

    struct tinyc_token *lparen = create_lparen(&span, 0);
    struct tinyc_token *ident1 = create_ident(&span, 0, "ident1");
    struct tinyc_token *rparen = create_rparen(&span, 0);
    struct tinyc_token *tokens = lparen, *head = tokens;
    tokens = tinyc_token_insert(tokens, ident1);
    tokens = tinyc_token_insert(tokens, rparen);

    struct tinyc_cpp_context ctx;
    tinyc_cpp_context_init(&ctx);

    struct tinyc_token *it = head;
    struct tinyc_cpp_macro_func_param *params;
    assert(tinyc_cpp_parse_params(&ctx, &repo, head, &it, &params));
    assert(tinyc_token_is_punct_of(it, TINYC_TOKEN_PUNCT_RPAREN));

    struct tinyc_cpp_macro_func_param *p0, *p1;
    p0 = tinyc_cpp_macro_func_params_at(params, 0);
    assert(p0 && strcmp(p0->name.cstr, "ident1") == 0);
    p1 = tinyc_cpp_macro_func_params_at(params, 1);
    assert(!p1);
}

static inline void two_params(void) {
    struct tinyc_repo repo;
    tinyc_repo_id id = dummy_repo(&repo);
    struct tinyc_span span = {
        id,
        {0, 0},
        {0, 0}
    };

    struct tinyc_token *lparen = create_lparen(&span, 0);
    struct tinyc_token *ident1 = create_ident(&span, 0, "ident1");
    struct tinyc_token *comma1 = create_comma(&span, 0);
    struct tinyc_token *ident2 = create_ident(&span, 0, "ident2");
    struct tinyc_token *rparen = create_rparen(&span, 0);
    struct tinyc_token *tokens = lparen, *head = tokens;
    tokens = tinyc_token_insert(tokens, ident1);
    tokens = tinyc_token_insert(tokens, comma1);
    tokens = tinyc_token_insert(tokens, ident2);
    tokens = tinyc_token_insert(tokens, rparen);

    struct tinyc_cpp_context ctx;
    tinyc_cpp_context_init(&ctx);

    struct tinyc_token *it = head;
    struct tinyc_cpp_macro_func_param *params;
    assert(tinyc_cpp_parse_params(&ctx, &repo, head, &it, &params));
    assert(tinyc_token_is_punct_of(it, TINYC_TOKEN_PUNCT_RPAREN));

    struct tinyc_cpp_macro_func_param *p0, *p1, *p2;
    p0 = tinyc_cpp_macro_func_params_at(params, 0);
    assert(p0 && strcmp(p0->name.cstr, "ident1") == 0);
    p1 = tinyc_cpp_macro_func_params_at(params, 1);
    assert(p1 && strcmp(p1->name.cstr, "ident2") == 0);
    p2 = tinyc_cpp_macro_func_params_at(params, 2);
    assert(!p2);
}

static inline void unclosing_no_param(void) {
    struct tinyc_repo repo;
    tinyc_repo_id id = dummy_repo(&repo);
    struct tinyc_span span = {
        id,
        {0, 0},
        {0, 0}
    };

    struct tinyc_token *lparen = create_lparen(&span, 0);
    struct tinyc_token *tokens = lparen, *head = tokens;

    struct tinyc_cpp_context ctx;
    tinyc_cpp_context_init(&ctx);

    struct tinyc_token *it = head;
    struct tinyc_cpp_macro_func_param *params;
    assert(!tinyc_cpp_parse_params(&ctx, &repo, head, &it, &params));
}

static inline void unclosing_one_param(void) {
    struct tinyc_repo repo;
    tinyc_repo_id id = dummy_repo(&repo);
    struct tinyc_span span = {
        id,
        {0, 0},
        {0, 0}
    };

    struct tinyc_token *lparen = create_lparen(&span, 0);
    struct tinyc_token *ident1 = create_ident(&span, 0, "ident1");
    struct tinyc_token *tokens = lparen, *head = tokens;
    tokens = tinyc_token_insert(tokens, ident1);

    struct tinyc_cpp_context ctx;
    tinyc_cpp_context_init(&ctx);

    struct tinyc_token *it = head;
    struct tinyc_cpp_macro_func_param *params;
    assert(!tinyc_cpp_parse_params(&ctx, &repo, head, &it, &params));
}

static inline void unclosing_two_params(void) {
    struct tinyc_repo repo;
    tinyc_repo_id id = dummy_repo(&repo);
    struct tinyc_span span = {
        id,
        {0, 0},
        {0, 0}
    };

    struct tinyc_token *lparen = create_lparen(&span, 0);
    struct tinyc_token *ident1 = create_ident(&span, 0, "ident1");
    struct tinyc_token *comma1 = create_comma(&span, 0);
    struct tinyc_token *ident2 = create_ident(&span, 0, "ident2");
    struct tinyc_token *tokens = lparen, *head = tokens;
    tokens = tinyc_token_insert(tokens, ident1);
    tokens = tinyc_token_insert(tokens, comma1);
    tokens = tinyc_token_insert(tokens, ident2);

    struct tinyc_cpp_context ctx;
    tinyc_cpp_context_init(&ctx);

    struct tinyc_token *it = head;
    struct tinyc_cpp_macro_func_param *params;
    assert(!tinyc_cpp_parse_params(&ctx, &repo, head, &it, &params));
}

static inline void missing_comman(void) {
    struct tinyc_repo repo;
    tinyc_repo_id id = dummy_repo(&repo);
    struct tinyc_span span = {
        id,
        {0, 0},
        {0, 0}
    };

    struct tinyc_token *lparen = create_lparen(&span, 0);
    struct tinyc_token *ident1 = create_ident(&span, 0, "ident1");
    struct tinyc_token *ident2 = create_ident(&span, 0, "ident2");
    struct tinyc_token *rparen = create_rparen(&span, 0);
    struct tinyc_token *tokens = lparen, *head = tokens;
    tokens = tinyc_token_insert(tokens, ident1);
    tokens = tinyc_token_insert(tokens, ident2);
    tokens = tinyc_token_insert(tokens, rparen);

    struct tinyc_cpp_context ctx;
    tinyc_cpp_context_init(&ctx);

    struct tinyc_token *it = head;
    struct tinyc_cpp_macro_func_param *params;
    assert(!tinyc_cpp_parse_params(&ctx, &repo, head, &it, &params));
}

static inline void mismatch_param_type(void) {
    struct tinyc_repo repo;
    tinyc_repo_id id = dummy_repo(&repo);
    struct tinyc_span span = {
        id,
        {0, 0},
        {0, 0}
    };

    struct tinyc_token *lparen = create_lparen(&span, 0);
    struct tinyc_token *plus = create_plus(&span, 0);
    struct tinyc_token *comma1 = create_comma(&span, 0);
    struct tinyc_token *ident2 = create_ident(&span, 0, "ident2");
    struct tinyc_token *rparen = create_rparen(&span, 0);
    struct tinyc_token *tokens = lparen, *head = tokens;
    tokens = tinyc_token_insert(tokens, plus);
    tokens = tinyc_token_insert(tokens, comma1);
    tokens = tinyc_token_insert(tokens, ident2);
    tokens = tinyc_token_insert(tokens, rparen);

    struct tinyc_cpp_context ctx;
    tinyc_cpp_context_init(&ctx);

    struct tinyc_token *it = head;
    struct tinyc_cpp_macro_func_param *params;
    assert(!tinyc_cpp_parse_params(&ctx, &repo, head, &it, &params));
}

int main(void) {
    no_param();
    one_param();
    two_params();
    unclosing_no_param();
    unclosing_one_param();
    unclosing_two_params();
    missing_comman();
    mismatch_param_type();
}
