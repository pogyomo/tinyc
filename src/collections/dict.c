#include "dict.h"

#include <stdlib.h>
#include <string.h>

#include "../panic.h"
#include "string.h"

#define DICT_INIT_CAP 100

static size_t hash(const char *s) {
    // FNV hash algorithm
    size_t len = strlen(s);
    size_t hash = 0xcbf29ce484222325;
    for (int i = 0; i < len; i++) {
        hash *= 0x100000001b3;
        hash ^= s[i];
    }
    return hash;
}

void dict_init(dict_t *dict, size_t size) {
    dict->size = size;
    dict->cap = DICT_INIT_CAP;
    dict->entries = calloc(1, sizeof(dict_entry_t *) * dict->cap);
}

void dict_insert(dict_t *dict, const char *key, const void *ptr) {
    size_t i = hash(key) % dict->cap;
    dict_entry_t *entry;
    if (dict->entries[i]) {
        entry = dict->entries[i];
        for (; entry->next; entry = entry->next) {
            if (entry->is_removed && strcmp(entry->key.str, key) == 0) {
                entry->is_removed = false;
                memcpy(entry->value, ptr, dict->size);
                return;
            }
        }
        entry->next = malloc(sizeof(dict_entry_t) + dict->size);
        if (!entry->next) panic_internal("failed to allocate memory");
        entry = entry->next;
    } else {
        dict->entries[i] = malloc(sizeof(dict_entry_t) + dict->size);
        if (!dict->entries[i]) panic_internal("failed to allocate memory");
        entry = dict->entries[i];
    }
    entry->next = NULL;
    entry->is_removed = false;
    string_from(&entry->key, key);
    memcpy(entry->value, ptr, dict->size);
}

void dict_remove(dict_t *dict, const char *key) {
    size_t i = hash(key) % dict->cap;
    dict_entry_t *entry = dict->entries[i];
    for (; entry; entry = entry->next) {
        if (strcmp(entry->key.str, key) == 0) {
            entry->is_removed = true;
            return;
        }
    }
}

void *dict_find(dict_t *dict, const char *key) {
    size_t i = hash(key) % dict->cap;
    dict_entry_t *entry = dict->entries[i];
    for (; entry; entry = entry->next) {
        if (!entry->is_removed && strcmp(entry->key.str, key) == 0) {
            return entry->value;
        }
    }
    return NULL;
}
