#include "token.h"

#include <string.h>

static bool startwith(const char *s, const char *prefix) {
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

bool token_is_int_octal(token_t *token) {
    return token->kind == TK_INTEGER && startwith(token->value->str, "0") &&
           !startwith(token->value->str, "0x");
}

bool token_is_int_decimal(token_t *token) {
    return token->kind == TK_INTEGER && !startwith(token->value->str, "0") &&
           !startwith(token->value->str, "0x");
}

bool token_is_int_hexadecimal(token_t *token) {
    return token->kind == TK_INTEGER && startwith(token->value->str, "0x");
}

bool token_is_float_decimal(token_t *token) {
    return token->kind == TK_FLOATING && !startwith(token->value->str, "0x");
}

bool token_is_float_hexadecimal(token_t *token) {
    return token->kind == TK_FLOATING && startwith(token->value->str, "0x");
}
