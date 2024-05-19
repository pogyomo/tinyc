#ifndef TINYC_COLLECTIONS_STRING_H_
#define TINYC_COLLECTIONS_STRING_H_

#include <stddef.h>

// A buffered string.
typedef struct {
    char *str;
    size_t cap;
    size_t len;
} string_t;

// Initialize `string` to be ready to use.
void string_init(string_t *string);

// Initialize `string` with `s`.
void string_from(string_t *string, const char *s);

// Initialize `string` with formatted string.
void string_format(string_t *string, const char *restrict format, ...);

// Push a character `c` to the tail of `string`.
void string_push(string_t *string, char c);

// Append string `s` to tail of `string`.
void string_append(string_t *string, const char *s);

// Pop a character from tail of `string`.
char string_pop(string_t *string);

// Get n-th character in `string`.
char string_at(const string_t *string, size_t n);

// Peek a character in tail of `string`.
char string_top(const string_t *string);

#endif  // TINYC_COLLECTIONS_STRING_H_
