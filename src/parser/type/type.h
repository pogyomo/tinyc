#ifndef TINYC_PARSER_TYPE_H_
#define TINYC_PARSER_TYPE_H_

#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

#include "../node.h"
#include "quantifier.h"
#include "specifier/specifier.h"

namespace tinyc {

class Type : public Node {
public:
    // If `specifiers` is empty, throw `runtime_error` exception.
    Type(const std::vector<std::shared_ptr<TypeSpecifier>>& specifiers,
         const std::vector<std::shared_ptr<TypeQuantifier>>& quantifiers)
        : specifiers_(specifiers), quantifiers_(quantifiers) {
        if (specifiers.empty()) {
            throw std::runtime_error("type must have at least one specifier");
        }
    }

    const std::vector<std::shared_ptr<TypeSpecifier>>& specifiers() const {
        return specifiers_;
    }

    const std::vector<std::shared_ptr<TypeQuantifier>>& quantifiers() const {
        return quantifiers_;
    }

    Span span() const override {
        std::vector<Span> spans;
        for (const auto& specifier : specifiers_) {
            spans.emplace_back(specifier->span());
        }
        for (const auto& quantifier : quantifiers_) {
            spans.emplace_back(quantifier->span());
        }
        return concat_spans(spans);
    }

private:
    const std::vector<std::shared_ptr<TypeSpecifier>> specifiers_;
    const std::vector<std::shared_ptr<TypeQuantifier>> quantifiers_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_TYPE_H_
