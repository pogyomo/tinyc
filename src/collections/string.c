#include "string.h"

#include <stdlib.h>
#include <string.h>

#include "../panic.h"

string_t *string_new() { return string_from_c_str(""); }

string_t *string_from_c_str(char *s) {
    int len = strlen(s);
    int cap = len + 100;
    char *str = malloc(sizeof(char) * cap);
    if (!str) panic_internal("failed to allocate memory");
    strcpy(str, s);

    string_t *string = malloc(sizeof(string_t));
    if (!string) panic("failed to allocate memory");
    string->str = str;
    string->cap = cap;
    string->len = len;
    return string;
}

void string_extend(string_t *string, size_t size) {
    string->cap += size;
    string->str = realloc(string->str, sizeof(char) * string->cap);
    if (!string->str) panic_internal("failed to extend string");
}

void string_push(string_t *string, char c) {
    if (string->len + 1 >= string->cap) {
        string_extend(string, 100);
    }
    string->str[string->len++] = c;
    string->str[string->len] = '\0';
}

void string_append(string_t *dst, const string_t *src) {
    if (dst->len + src->len + 1 >= dst->cap) {
        string_extend(dst, src->len + 100);
    }
    strcpy(&dst->str[dst->len], src->str);
    dst->len += src->len;
}

char string_pop(string_t *string) {
    if (string->len == 0) panic_internal("pop from empty string");
    char c = string->str[--string->len];
    string->str[string->len] = '\0';
    return c;
}

char string_at(string_t *string, size_t n) {
    if (n >= string->len) panic_internal("at called with exceed index");
    return string->str[n];
}

char string_top(string_t *string) {
    if (string->len == 0) panic_internal("top from empty string");
    return string->str[string->len - 1];
}
