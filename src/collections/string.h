#ifndef TINYC_COLLECTIONS_STRING_H_
#define TINYC_COLLECTIONS_STRING_H_

#include <stdlib.h>

// A buffered string which can push/pop character from it.
typedef struct {
    char *str;
    int cap;
    int len;
} string_t;

// Construct an empty string.
string_t *string_new();

// Construct string from null-terminated string `s`.
string_t *string_from_c_str(char *s);

// Extend this string so that extra `size` character this vector can hold.
void string_extend(string_t *string, size_t size);

// Push a character to string.
void string_push(string_t *string, char c);

// Append `src` to `dst`.
void string_append(string_t *dst, const string_t *src);

// Pop a character from string.
char string_pop(string_t *string);

// Get the character in specified index.
char string_at(string_t *string, size_t n);

// Peek a top element in string.
char string_top(string_t *string);

#endif  // TINYC_COLLECTIONS_STRING_H_
