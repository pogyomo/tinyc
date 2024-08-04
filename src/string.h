// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_STRING_H_
#define TINYC_STRING_H_

#include <stddef.h>

struct string {
    char *str;   // A null-terminated string.
    size_t cap;  // How many character can `str` holds.
    size_t len;  // How many character is in `str`.
};

// Initialize `s`. Should be called before use it.
void string_init(struct string *s);

// Initialize `s` with `from`.
void string_from(struct string *s, const char *from);

// Push a character.
void string_push(struct string *s, char c);

// Append `src` to `dst`.
void string_append(struct string *dst, const char *src);

// Pop a character from `s`.
// If `s` is empty, cause error.
void string_pop(struct string *s);

// Peek a character in top of `s`.
// If `s` is empty, cause error.
char string_top(const struct string *s);

// Returns `n`-th character in `s`.
// If `n` exceed the length of `s`, cause error.
char string_at(const struct string *s, size_t n);

#endif  // TINYC_STRING_H_
