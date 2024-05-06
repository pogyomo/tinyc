#include "type.h"

#include "../../span.h"
#include "../expr.h"

namespace tinyc {

Span ArrayType::span() const {
    std::vector<Span> spans;
    spans.emplace_back(of_->span());
    spans.emplace_back(lsquare_.span());
    if (size_.has_value()) spans.emplace_back(size_.value()->span());
    spans.emplace_back(rsquare_.span());
    return concat_spans(spans);
}

std::string ArrayType::debug() const {
    if (size_.has_value()) {
        return "(" + of_->debug() + ")" + lsquare_.debug() +
               size_.value()->debug() + rsquare_.debug();
    } else {
        return "(" + of_->debug() + ")" + lsquare_.debug() + rsquare_.debug();
    }
}

}  // namespace tinyc
