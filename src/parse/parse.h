// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_PARSE_H_
#define TINYC_PARSE_PARSE_H_

#include <stdbool.h>

#include "../context.h"
#include "ast/unit.h"

// Convert file into translation unit: list of definition and declaration.
// Returns false if error happen.
bool parse_file(struct context *ctx, const char *path,
                struct trans_unit **units);

#endif  // TINYC_PARSE_PARSE_H_
