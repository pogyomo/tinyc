#include <stdio.h>

#include "context.h"
#include "lex/lex.h"

int main() {
    struct context ctx;
    context_init(&ctx);

    struct token *tokens;
    if (!lex_file(&ctx, "main.c", &tokens)) return 1;
    token_iter(token, tokens) {
        printf("%d:%zu:%zu\n", token->kind, token->span.start.row,
               token->span.start.col);
    }
}
