#include "ast.h"

#include "../memory.h"

expr_t *expr_alloc() { return malloc_panic(sizeof(expr_t)); }

stmt_t *stmt_alloc() { return malloc_panic(sizeof(stmt_t)); }

decl_t *decl_alloc() { return malloc_panic(sizeof(decl_t)); }

type_t *type_alloc() { return malloc_panic(sizeof(type_t)); }
