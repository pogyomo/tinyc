#include "string.h"

#include <stdlib.h>
#include <string.h>

#include "../panic.h"

// Make `string->str` point to heap so that modification to it become possible.
static void heaplify(string_t *string) {
    if (!string->is_static) return;

    string->cap = string->len + 101;
    string->is_static = false;

    char *str = string->str;
    string->str = malloc(sizeof(char) * string->cap);
    if (!string->str) panic_internal("failed to allocate memory");
    strcpy(string->str, str);
}

string_t *string_new() { return string_from(""); }

string_t *string_from(char *s) {
    string_t *string = malloc(sizeof(string_t));
    if (!string) panic("failed to allocate memory");
    string->str = s;
    string->cap = 0;
    string->len = strlen(s);
    string->is_static = true;
    return string;
}

void string_extend(string_t *string, size_t size) {
    if (string->is_static) heaplify(string);
    string->cap += size;
    string->str = realloc(string->str, sizeof(char) * string->cap);
    if (!string->str) panic_internal("failed to extend string");
}

void string_push(string_t *string, char c) {
    if (string->is_static) heaplify(string);
    if (string->len + 1 >= string->cap) {
        string_extend(string, 100);
    }
    string->str[string->len++] = c;
    string->str[string->len] = '\0';
}

void string_append(string_t *dst, const char *src) {
    size_t src_len = strlen(src);
    for (int i = 0; i < src_len; i++) {
        string_push(dst, src[i]);
    }
    /*
    size_t src_len = strlen(src);
    if (dst->is_static) heaplify(dst);
    if (dst->len + src_len + 1 >= dst->cap) {
        string_extend(dst, src_len + 100);
    }
    strcpy(&dst->str[dst->len], src);
    dst->len += src_len;
    */
}

char string_pop(string_t *string) {
    if (string->is_static) heaplify(string);
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
