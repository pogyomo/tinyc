#ifndef PCC_PARSE_PARSE_H_
#define PCC_PARSE_PARSE_H_

#include <stdbool.h>

#include "../context.h"

bool parse_file(struct context *ctx, const char *path);

#endif  // PCC_PARSE_PARSE_H_
