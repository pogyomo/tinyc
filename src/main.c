#include <stdio.h>

#include "collections/vector.h"
#include "context.h"
#include "lexer/lexer.h"
#include "lexer/token.h"

int main() {
    context_t ctx;
    context_init(&ctx);
    vector_t tokens;
    if (!lex_file(&ctx, "a.c", &tokens)) {
        return 1;
    }
    for (int i = 0; i < tokens.len; i++) {
        token_t *token = vector_at(&tokens, i);
        printf("%s ", token->printable.str);
    }
}
