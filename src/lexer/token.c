#include "token.h"

#include "../panic.h"

string_t token_string_inner(token_t *token) {
    if (token->kind != TK_STRING) panic_internal("constraint is not hold");
    string_t result;
    string_init(&result);
    for (int i = 1; i < token->repr.len - 1; i++) {
        string_push(&result, string_at(&token->repr, i));
    }
    return result;
}

string_t token_character_inner(token_t *token) {
    if (token->kind != TK_CHARACTER) panic_internal("constraint is not hold");
    string_t result;
    string_init(&result);
    for (int i = 1; i < token->repr.len - 1; i++) {
        string_push(&result, string_at(&token->repr, i));
    }
    return result;
}
