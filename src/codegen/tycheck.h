#ifndef TINYC_CODEGEN_TYCHECK_H_
#define TINYC_CODEGEN_TYCHECK_H_

#include <memory>
#include <optional>

#include "../context.h"
#include "../parser/expr.h"

namespace tinyc {

// Assert given expression's type.
// If failed, report error and return std::nullopt.
std::optional<std::shared_ptr<Type>> tyassert(
    Context& ctx, const std::shared_ptr<Expression>& expr);

// Check if `rhs` is equal or convertable to `lhs`.
// If failed, report error and returns false.
bool tycheck(Context& ctx, const std::shared_ptr<Type>& lhs,
             const std::shared_ptr<Type>& rhs);

}  // namespace tinyc

#endif  // TINYC_CODEGEN_TYCHECK_H_
