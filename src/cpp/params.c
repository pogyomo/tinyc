#include "tinyc/cpp/params.h"

#include <assert.h>

#include "tinyc/cpp/helper.h"
#include "tinyc/cpp/macro.h"
#include "tinyc/diag.h"
#include "tinyc/string.h"
#include "tinyc/token.h"

bool tinyc_cpp_parse_params(
    struct tinyc_cpp_context *ctx,
    const struct tinyc_repo *repo,
    struct tinyc_token *head,
    struct tinyc_token **it,
    struct tinyc_cpp_macro_func_param **params
) {
    assert(tinyc_token_is_punct_of(*it, TINYC_TOKEN_PUNCT_LPAREN));

    if (!tinyc_cpp_expect_token_next(repo, head, it)) return false;

    *params = NULL;
    for (size_t index = 0;; ++index) {
        if (tinyc_token_is_punct_of(*it, TINYC_TOKEN_PUNCT_RPAREN)) {
            break;
        } else if (!tinyc_cpp_expect_ident(repo, *it)) {
            return false;
        }

        struct tinyc_token_ident *name = (struct tinyc_token_ident *)(*it);
        struct tinyc_cpp_macro_func_param *param;
        param = tinyc_cpp_macro_func_param_create(name->value.cstr, index);
        if (*params) param->next = *params;
        *params = param;

        if (!tinyc_cpp_expect_token_next(repo, head, it)) return false;
        if (tinyc_token_is_punct_of(*it, TINYC_TOKEN_PUNCT_RPAREN)) {
            break;
        } else if (tinyc_token_is_punct_of(*it, TINYC_TOKEN_PUNCT_COMMA)) {
            if (!tinyc_cpp_expect_token_next(repo, head, it)) return false;
        } else {
            tinyc_diag(
                TINYC_DIAG_ERROR,
                repo,
                &(*it)->span,
                "unknown token found",
                "expect this to be ) or ,"
            );
            return false;
        }
    }
    return true;
}
