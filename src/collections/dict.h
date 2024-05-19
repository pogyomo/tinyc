#ifndef TINYC_COLLECTIONS_DICT_H_
#define TINYC_COLLECTIONS_DICT_H_

#include <stdbool.h>

#include "string.h"

// An entry of dictionary.
typedef struct dict_entry {
    struct dict_entry *next;
    bool is_removed;
    string_t key;
    char value[];
} dict_entry_t;

// A dictionary which can access to value by string.
typedef struct {
    dict_entry_t **entries;
    size_t cap;
    size_t size;
} dict_t;

// Initialize `dict` to be ready to use.
void dict_init(dict_t *dict, size_t size);

// Insert key-value pair to `dict` where value is the value inside of `ptr`.
void dict_insert(dict_t *dict, const char *key, const void *ptr);

// Remove value associated with `key` from `dict`.
void dict_remove(dict_t *dict, const char *key);

// Find value associated with `key` from `dict` and return reference to it.
// If no such value exist, return NULL.
void *dict_find(dict_t *dict, const char *key);

#endif  // TINYC_COLLECTIONS_DICT_H_
