#include "span.h"

#include <climits>
#include <stdexcept>

namespace tinyc {

Span concat_spans(const std::vector<Span>& spans) {
    if (spans.empty()) {
        throw std::runtime_error("empty `spans` was passed to `concat_spans`");
    }
    const int id = spans.front().id();
    Position start = spans.front().start();
    Position end = spans.front().end();
    for (const auto span : spans) {
        if (id != span.id()) {
            throw std::runtime_error("different id found in `spans`");
        }
        if (start > span.start()) {
            start = span.start();
        }
        if (end < span.end()) {
            end = span.end();
        }
    }
    return Span(id, start, end);
}

}  // namespace tinyc
