#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../collections/string.h"
#include "../collections/vector.h"
#include "../panic.h"

string_t *extract_filename(string_t *path) {
    char *filename = strrchr(path->str, '/');
    if (!filename) panic("no file name in '%s'", path->str);
    filename++;
    return string_from_c_str(filename);
}

input_t *input_from_file(string_t *path) {
    FILE *fp = fopen(path->str, "r");
    if (!fp) panic("failed to open '%s'", path->str);

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
    input->name = extract_filename(path);
    return input;
}

string_t *input_get_line(input_t *input, size_t row) {
    if (row >= input->lines->len) panic_internal("exceed index");
    return vector_at(input->lines, row);
}
