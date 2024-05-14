#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../collections/string.h"
#include "../collections/vector.h"
#include "../report/error.h"
#include "../report/panic.h"

string_t *extract_filename(string_t *path) {
    char *filename = strrchr(path->str, '/');
    if (!filename) error("no file name in %s", path->str);
    return string_from_str(filename);
}

input_t *input_from_file(string_t *path) {
    FILE *fp = fopen(path->str, "r");
    if (!fp) error("failed to open %s", path->str);

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
    if (!input) panic("failed to allocate memory");
    input->lines = lines;
    input->filename = extract_filename(path);
    return input;
}

string_t *input_get_line(input_t *input, size_t row) {
    if (row >= input->lines->len) panic("exceed index");
    return vector_at(input->lines, row);
}
