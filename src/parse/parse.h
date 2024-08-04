// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_PARSE_H_
#define TINYC_PARSE_PARSE_H_

#include <stdbool.h>

#include "../context.h"

bool parse_file(struct context *ctx, const char *path);

#endif  // TINYC_PARSE_PARSE_H_
