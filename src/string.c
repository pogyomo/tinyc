#include "string.h"

#include <stdlib.h>
#include <string.h>

#include "panic.h"

#define INIT_CAP 100

static void extend(struct string *s, size_t atleast) {
    if (s->cap + s->cap < atleast) s->cap = atleast;
    s->cap += s->cap;
    s->str = realloc(s->str, s->cap);
    if (!s->str) fatal_error("memory reallocation failed");
}

void string_init(struct string *s) {
    s->cap = INIT_CAP;
    s->len = 0;
    s->str = malloc(sizeof(char) * s->cap);
    if (!s->str) fatal_error("memory allocation failed");
    s->str[0] = '\0';
}

void string_from(struct string *s, const char *from) {
    s->len = strlen(from);
    s->cap = s->len + 1;
    s->str = malloc(sizeof(char) * s->cap);
    if (!s->str) fatal_error("memory allocation failed");
    strcpy(s->str, from);
}

void string_push(struct string *s, char c) {
    if (s->len + 2 > s->cap) extend(s, s->len + 2);
    s->str[s->len++] = c;
    s->str[s->len] = '\0';
}

void string_append(struct string *dst, const char *src) {
    size_t len = strlen(src);
    if (dst->len + len + 1 > dst->cap) extend(dst, dst->len + len + 1);
    strcpy(dst->str + dst->len, src);
    dst->len += len;
}

void string_pop(struct string *s) {
    if (s->len == 0) fatal_error("empty string");
    s->str[--s->len] = '\0';
}

char string_top(const struct string *s) {
    if (s->len == 0) fatal_error("empty string");
    return s->str[s->len - 1];
}

char string_at(const struct string *s, size_t n) {
    if (n >= s->len) fatal_error("index exceed");
    return s->str[n];
}
