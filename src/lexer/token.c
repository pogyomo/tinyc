#include "token.h"

#include "../panic.h"

string_t token_string_inner(token_t *token) {
    if (token->kind != TK_STRING) panic_internal("constraint is not hold");
    string_t result;
    string_from(&result, &token->repr.str[1]);
    string_pop(&result);
    return result;
}

string_t token_character_inner(token_t *token) {
    if (token->kind != TK_CHARACTER) panic_internal("constraint is not hold");
    string_t result;
    string_from(&result, &token->repr.str[1]);
    string_pop(&result);
    return result;
}
