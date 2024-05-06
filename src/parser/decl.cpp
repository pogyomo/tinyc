#include "decl.h"

#include "stmt.h"

namespace tinyc {

Span FunctionDeclaration::span() const {
    std::vector<Span> spans;
    spans.emplace_back(ret_type_->span());
    spans.emplace_back(name_.span());
    spans.emplace_back(lparen_.span());
    for (const auto& arg : args_) spans.emplace_back(arg.span());
    spans.emplace_back(rparen_.span());
    if (body_.has_value()) spans.emplace_back(body_.value()->span());
    return concat_spans(spans);
}

}  // namespace tinyc
