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

std::string FunctionDeclaration::debug() const {
    std::stringstream ss;
    ss << ret_type_->debug() << " " << name_.debug() << lparen_.debug();
    if (!args_.empty()) {
        ss << args_[0].debug();
        for (int i = 1; i < args_.size(); i++) ss << ", " << args_[i].debug();
    }
    ss << rparen_.debug() << " ";
    if (body_.has_value()) ss << body_.value()->debug();
    if (semicolon_.has_value()) ss << semicolon_->debug();
    return ss.str();
}

}  // namespace tinyc
