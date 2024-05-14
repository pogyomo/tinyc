#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../collections/string.h"
#include "../collections/vector.h"
#include "../panic.h"

static char *extract_filename(char *path) {
    char *filename = strrchr(path, '/');
    if (!filename) return path;
    return ++filename;
}

input_t *input_from_file(char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) panic("failed to open '%s'", path);

    char c;
    vector_t *lines = vector_new();
    string_t *line = string_new();
    while (!feof(fp)) {
        if ((c = fgetc(fp)) == '\n') {
            vector_push(lines, line);
            line = string_new();
        } else {
            string_push(line, c);
        }
    }
    fclose(fp);

    input_t *input = malloc(sizeof(input_t));
    if (!input) panic_internal("failed to allocate memory");
    input->lines = lines;
    input->name = string_from(extract_filename(path));
    return input;
}

string_t *input_get_line(input_t *input, size_t row) {
    if (row >= input->lines->len) panic_internal("exceed index");
    return vector_at(input->lines, row);
}
