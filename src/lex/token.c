#include "token.h"

#include <stdlib.h>

#include "../panic.h"

struct token *token_new(enum token_kind kind, struct span *span) {
    struct token *token = malloc(sizeof(struct token));
    if (!token) fatal_error("memory allocation failed");
    token->next = NULL;
    token->kind = kind;
    token->span = *span;
    return token;
}
