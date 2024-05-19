#include "string.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../panic.h"

#define STRING_INIT_CAP 100
#define STRING_EXTEND_SIZE 100

// Extend `string` so that extra `size` character the string can hold.
static void extend(string_t *string, size_t size) {
    string->cap += size;
    string->str = realloc(string->str, string->cap);
    if (!string->str) panic_internal("failed extend string");
}

void string_init(string_t *string) {
    string->len = 0;
    string->cap = STRING_INIT_CAP + 1;
    string->str = malloc(sizeof(char) * string->cap);
    if (!string->str) panic_internal("failed to allocate memory");
    string->str[0] = '\0';
}

void string_from(string_t *string, const char *s) {
    string->len = strlen(s);
    string->cap = string->len + 1;
    string->str = malloc(sizeof(char) * string->cap);
    if (!string->str) panic_internal("failed to allocate memory");
    strcpy(string->str, s);
}

void string_format(string_t *string, const char *restrict format, ...) {
    va_list arg;

    va_start(arg, format);
    int len = vsnprintf(NULL, 0, format, arg);
    va_end(arg);
    if (len < 0) panic_internal("encoding error happen");

    size_t cap = len + 1;
    char *buffer = malloc(cap);
    va_start(arg, format);
    vsnprintf(buffer, cap, format, arg);
    va_end(arg);

    string->len = strlen(buffer);
    string->cap = cap;
    string->str = buffer;
}

void string_push(string_t *string, char c) {
    if (string->len + 1 + 1 >= string->cap)
        extend(string, 1 + STRING_EXTEND_SIZE);
    string->str[string->len++] = c;
    string->str[string->len] = '\0';
}

void string_append(string_t *string, const char *s) {
    size_t len = strlen(s);
    if (string->len + len + 1 >= string->cap)
        extend(string, len + STRING_EXTEND_SIZE);
    strcpy(string->str + string->len, s);
}

char string_pop(string_t *string) {
    if (string->len == 0) panic_internal("pop from empty string");
    char c = string->str[--string->len];
    string->str[string->len] = '\0';
    return c;
}

char string_at(const string_t *string, size_t n) {
    if (n >= string->len) panic_internal("index exceed");
    return string->str[n];
}

char string_top(const string_t *string) {
    if (string->len == 0) panic_internal("top from empty string");
    return string->str[string->len - 1];
}
