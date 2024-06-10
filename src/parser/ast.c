#include "ast.h"

#include "../memory.h"

inline expr_t *expr_alloc() { return malloc_panic(sizeof(expr_t)); }

inline stmt_t *stmt_alloc() { return malloc_panic(sizeof(stmt_t)); }

inline decl_t *decl_alloc() { return malloc_panic(sizeof(decl_t)); }

inline type_t *type_alloc() { return malloc_panic(sizeof(type_t)); }
