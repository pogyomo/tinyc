#ifndef TINYC_PARSER_PARSER_H_
#define TINYC_PARSER_PARSER_H_

#include <stdbool.h>

#include "../context.h"

// Parse given file as list of declaration and initialize `decls` with it.
// Returns false if error happen.
bool parse_file(context_t *ctx, char *path, vector_t *decls);

#endif  // TINYC_PARSER_PARSER_H_
