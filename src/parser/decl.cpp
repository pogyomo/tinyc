#include "decl.h"

#include "stmt.h"

namespace tinyc {

std::string StorageClassSpecifier::debug() const {
    switch (kind_) {
        case StorageClassSpecifierKind::Auto:
            return "auto";
        case StorageClassSpecifierKind::Register:
            return "register";
        case StorageClassSpecifierKind::Static:
            return "static";
        case StorageClassSpecifierKind::Extern:
            return "extern";
        case StorageClassSpecifierKind::Typedef:
            return "typedef";
        default:
            return "";
    }
}

Span VariableDeclarationInitializer::span() const {
    return concat_spans({assign_.span(), expr_->span()});
}

std::string VariableDeclarationInitializer::debug() const {
    return assign_.debug() + " " + expr_->debug();
}

Span FunctionDeclaration::span() const {
    std::vector<Span> spans;
    for (const auto& cs : class_specifiers_) spans.emplace_back(cs.span());
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
    for (const auto& cs : class_specifiers_) ss << cs.debug() << " ";
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
