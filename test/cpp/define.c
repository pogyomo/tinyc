#include <assert.h>
#include <string.h>
#include <tinyc/cpp/define.h>

#include "tinyc/cpp/context.h"
#include "tinyc/cpp/macro.h"
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
    return tinyc_token_create_punct(span, tspaces, TINYC_TOKEN_PUNCT_LPAREN);
}

static inline struct tinyc_token *create_rparen(
    const struct tinyc_span *span,
    int tspaces
) {
    return tinyc_token_create_punct(span, tspaces, TINYC_TOKEN_PUNCT_RPAREN);
}

static inline struct tinyc_token *create_ident(
    const struct tinyc_span *span,
    int tspaces,
    const char *value
) {
    return tinyc_token_create_ident(span, tspaces, value);
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

static inline void normal(void) {
    struct tinyc_repo repo;
    tinyc_repo_id id = dummy_repo(&repo);
    struct tinyc_span span = {
        id,
        {0, 0},
        {0, 0}
    };

    struct tinyc_token *name = create_ident(&span, 1, "name");
    struct tinyc_token *ident1 = create_ident(&span, 1, "ident1");
    struct tinyc_token *plus = create_plus(&span, 1);
    struct tinyc_token *ident2 = create_ident(&span, 1, "ident2");
    struct tinyc_token *token = name, *head = token;
    token = tinyc_token_insert(token, ident1);
    token = tinyc_token_insert(token, plus);
    token = tinyc_token_insert(token, ident2);

    struct tinyc_cpp_context ctx;
    tinyc_cpp_context_init(&ctx);

    struct tinyc_token *it = head;
    assert(tinyc_cpp_parse_define(&ctx, &repo, head, &it));
    assert(it == head->prev);

    const struct tinyc_cpp_macro *macro;
    macro = tinyc_cpp_context_get_macro(&ctx, "name");
    assert(macro);
    assert(macro->kind == TINYC_CPP_MACRO_NORMAL);
    assert(macro->value);
    assert(tinyc_token_is_ident_of(macro->value, "ident1"));
    assert(tinyc_token_is_punct_of(macro->value->next, TINYC_TOKEN_PUNCT_PLUS));
    assert(tinyc_token_is_ident_of(macro->value->next->next, "ident2"));
    assert(tinyc_token_is_ident_of(macro->value->next->next->next, "ident1"));
}

static inline void normal_nobody(void) {
    struct tinyc_repo repo;
    tinyc_repo_id id = dummy_repo(&repo);
    struct tinyc_span span = {
        id,
        {0, 0},
        {0, 0}
    };

    struct tinyc_token *name = create_ident(&span, 1, "name");
    struct tinyc_token *token = name, *head = token;

    struct tinyc_cpp_context ctx;
    tinyc_cpp_context_init(&ctx);

    struct tinyc_token *it = head;
    assert(tinyc_cpp_parse_define(&ctx, &repo, head, &it));
    assert(it == head->prev);

    const struct tinyc_cpp_macro *macro;
    macro = tinyc_cpp_context_get_macro(&ctx, "name");
    assert(macro);
    assert(macro->kind == TINYC_CPP_MACRO_NORMAL);
    assert(!macro->value);
}

static inline void func(void) {
    struct tinyc_repo repo;
    tinyc_repo_id id = dummy_repo(&repo);
    struct tinyc_span span = {
        id,
        {0, 0},
        {0, 0}
    };

    struct tinyc_token *name = create_ident(&span, 0, "name");
    struct tinyc_token *lparen = create_lparen(&span, 0);
    struct tinyc_token *p1 = create_ident(&span, 0, "p1");
    struct tinyc_token *comma = create_comma(&span, 1);
    struct tinyc_token *p2 = create_ident(&span, 0, "p2");
    struct tinyc_token *rparen = create_rparen(&span, 1);
    struct tinyc_token *ident1 = create_ident(&span, 1, "ident1");
    struct tinyc_token *plus = create_plus(&span, 1);
    struct tinyc_token *ident2 = create_ident(&span, 1, "ident2");
    struct tinyc_token *token = name, *head = token;
    token = tinyc_token_insert(token, lparen);
    token = tinyc_token_insert(token, p1);
    token = tinyc_token_insert(token, comma);
    token = tinyc_token_insert(token, p2);
    token = tinyc_token_insert(token, rparen);
    token = tinyc_token_insert(token, ident1);
    token = tinyc_token_insert(token, plus);
    token = tinyc_token_insert(token, ident2);

    struct tinyc_cpp_context ctx;
    tinyc_cpp_context_init(&ctx);

    struct tinyc_token *it = head;
    assert(tinyc_cpp_parse_define(&ctx, &repo, head, &it));
    assert(it == head->prev);

    const struct tinyc_cpp_macro *macro;
    const struct tinyc_cpp_macro_func *func;
    macro = tinyc_cpp_context_get_macro(&ctx, "name");
    assert(macro);
    assert(tinyc_token_is_ident_of(macro->value, "ident1"));
    assert(tinyc_token_is_punct_of(macro->value->next, TINYC_TOKEN_PUNCT_PLUS));
    assert(tinyc_token_is_ident_of(macro->value->next->next, "ident2"));
    assert(tinyc_token_is_ident_of(macro->value->next->next->next, "ident1"));

    assert(macro->kind == TINYC_CPP_MACRO_FUNC);
    func = (struct tinyc_cpp_macro_func *)macro;

    struct tinyc_cpp_macro_func_param *param;
    param = tinyc_cpp_macro_func_params_at(func->params, 0);
    assert(param && strcmp(param->name.cstr, "p1") == 0);
    param = tinyc_cpp_macro_func_params_at(func->params, 1);
    assert(param && strcmp(param->name.cstr, "p2") == 0);
    param = tinyc_cpp_macro_func_params_at(func->params, 2);
    assert(!param);
}

static inline void func_nobody(void) {
    struct tinyc_repo repo;
    tinyc_repo_id id = dummy_repo(&repo);
    struct tinyc_span span = {
        id,
        {0, 0},
        {0, 0}
    };

    struct tinyc_token *name = create_ident(&span, 0, "name");
    struct tinyc_token *lparen = create_lparen(&span, 0);
    struct tinyc_token *p1 = create_ident(&span, 0, "p1");
    struct tinyc_token *comma = create_comma(&span, 1);
    struct tinyc_token *p2 = create_ident(&span, 0, "p2");
    struct tinyc_token *rparen = create_rparen(&span, 1);
    struct tinyc_token *token = name, *head = token;
    token = tinyc_token_insert(token, lparen);
    token = tinyc_token_insert(token, p1);
    token = tinyc_token_insert(token, comma);
    token = tinyc_token_insert(token, p2);
    token = tinyc_token_insert(token, rparen);

    struct tinyc_cpp_context ctx;
    tinyc_cpp_context_init(&ctx);

    struct tinyc_token *it = head;
    assert(tinyc_cpp_parse_define(&ctx, &repo, head, &it));
    assert(it == head->prev);

    const struct tinyc_cpp_macro *macro;
    const struct tinyc_cpp_macro_func *func;
    macro = tinyc_cpp_context_get_macro(&ctx, "name");
    assert(macro);
    assert(!macro->value);

    assert(macro->kind == TINYC_CPP_MACRO_FUNC);
    func = (struct tinyc_cpp_macro_func *)macro;

    struct tinyc_cpp_macro_func_param *param;
    param = tinyc_cpp_macro_func_params_at(func->params, 0);
    assert(param && strcmp(param->name.cstr, "p1") == 0);
    param = tinyc_cpp_macro_func_params_at(func->params, 1);
    assert(param && strcmp(param->name.cstr, "p2") == 0);
    param = tinyc_cpp_macro_func_params_at(func->params, 2);
    assert(!param);
}

static inline void func_spaces_params(void) {
    struct tinyc_repo repo;
    tinyc_repo_id id = dummy_repo(&repo);
    struct tinyc_span span = {
        id,
        {0, 0},
        {0, 0}
    };

    struct tinyc_token *name = create_ident(&span, 1, "name");
    struct tinyc_token *lparen = create_lparen(&span, 0);
    struct tinyc_token *p1 = create_ident(&span, 0, "p1");
    struct tinyc_token *comma = create_comma(&span, 1);
    struct tinyc_token *p2 = create_ident(&span, 0, "p2");
    struct tinyc_token *rparen = create_rparen(&span, 1);
    struct tinyc_token *token = name, *head = token;
    token = tinyc_token_insert(token, lparen);
    token = tinyc_token_insert(token, p1);
    token = tinyc_token_insert(token, comma);
    token = tinyc_token_insert(token, p2);
    token = tinyc_token_insert(token, rparen);

    struct tinyc_cpp_context ctx;
    tinyc_cpp_context_init(&ctx);

    struct tinyc_token *it = head;
    assert(tinyc_cpp_parse_define(&ctx, &repo, head, &it));
    assert(it == head->prev);

    const struct tinyc_cpp_macro *macro;
    const struct tinyc_cpp_macro_func *func;
    macro = tinyc_cpp_context_get_macro(&ctx, "name");
    assert(macro);
    assert(macro->kind == TINYC_CPP_MACRO_NORMAL);
    assert(macro->value);

    struct tinyc_token *value = macro->value;
    assert(tinyc_token_is_punct_of(value, TINYC_TOKEN_PUNCT_LPAREN));
    value = value->next;
    assert(tinyc_token_is_ident_of(value, "p1"));
    value = value->next;
    assert(tinyc_token_is_punct_of(value, TINYC_TOKEN_PUNCT_COMMA));
    value = value->next;
    assert(tinyc_token_is_ident_of(value, "p2"));
    value = value->next;
    assert(tinyc_token_is_punct_of(value, TINYC_TOKEN_PUNCT_RPAREN));
    value = value->next;
    assert(tinyc_token_is_punct_of(value, TINYC_TOKEN_PUNCT_LPAREN));
}

int main(void) {
    normal();
    normal_nobody();
    func();
    func_nobody();
    func_spaces_params();
}
