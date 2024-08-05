// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_UTILS_H_
#define TINYC_PARSE_UTILS_H_

#include "ast.h"
#include "context.h"
#include "stream.h"

// Extract type and storage classes from `ts`, then initialize `*type` and
// `*classes` with it.
// Returns false if no type available.
bool parse_type_base(struct parse_context *ctx, struct tstream *ts,
                     struct type **type, struct storage_class **classes);

#endif  // TINYC_PARSE_UTILS_H_
