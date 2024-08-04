#include "token.h"

#include <stdlib.h>

#include "../panic.h"

struct cpp_token *cpp_token_new(enum cpp_token_kind kind, struct span *span) {
    struct cpp_token *token = malloc(sizeof(struct cpp_token));
    if (!token) fatal_error("memory allocation faield");
    token->next = NULL;
    token->kind = kind;
    token->span = *span;
    return token;
}
