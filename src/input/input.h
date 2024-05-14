#ifndef TINYC_INPUT_INPUT_H_
#define TINYC_INPUT_INPUT_H_

#include "../collections/string.h"
#include "../collections/vector.h"

typedef struct {
    vector_t *lines;
    string_t *name;
} input_t;

// Construct a new input from file specified by `path`.
input_t *input_from_file(string_t *path);

// Returns `row`-th line of `input`.
string_t *input_get_line(input_t *input, size_t row);

#endif  // TINYC_INPUT_INPUT_H_
