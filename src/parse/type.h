// Copyrignts (C) 2024 pogyomo. Released under the MIT license.

#ifndef TINYC_PARSE_TYPE_H_
#define TINYC_PARSE_TYPE_H_

#include <stdbool.h>

#include "ast/decl.h"
#include "ast/type.h"
#include "context.h"
#include "stream.h"

// Parse type-name, then update `*type` with it.
// Returns false if error happen.
bool parse_type_name(struct parse_context *ctx, struct tstream *ts,
                     struct type **type);

// Parse type from `ts` which appear in lhs of declaration/definition, and then
// initialize `*type` with it.
// This also, update `*class` and `*func_spec` if storage-class-specifier and
// function-specifier found. `*class` and `*func_spec` will be NULL if not
// found.
// If you pass NULL to `class` or `func_spec`, you can reject it.
// Returns false if error happen.
bool parse_type_head(struct parse_context *ctx, struct tstream *ts,
                     struct type **type, struct storage_class **class,
                     struct function_spec **func_spec);

// Name appear in after base type.
struct type_rest_name {
    bool exists;
    char *name;
    struct span span;
};

// Parse type from `ts` which appear in rhs of declaration/definition, and then
// initialize `*type` with it and `head`.
// This also extract the name and save it to `name`. If no name found,
// `name->exists` will be false. If `name` is NULL, and name found, cause
// error.
// Returns false if error happen.
bool parse_type_rest(struct parse_context *ctx, struct tstream *ts,
                     struct type *head, struct type **type,
                     struct type_rest_name *name);

#endif  // TINYC_PARSE_TYPE_H_
