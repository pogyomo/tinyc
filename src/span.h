#ifndef TINYC_SPAN_H_
#define TINYC_SPAN_H_

#include <utility>

namespace tinyc {

class Span {
public:
    Span(int start_row, int start_offset, int end_row, int end_offset)
        : start_({start_row, start_offset}), end_({end_row, end_offset}) {}
    Span(std::pair<int, int> start, std::pair<int, int> end)
        : start_(start), end_(end) {}

    // Returns pair of start row and offset of this span.
    inline std::pair<int, int> start() const { return start_; }

    // Returns pair of end row and offset of this span.
    inline std::pair<int, int> end() const { return end_; }

    // Returns start row of this span.
    inline int start_row() const { return start_.first; }

    // Returns start offset of this span.
    inline int start_offset() const { return start_.second; }

    // Returns end row of this span.
    inline int end_row() const { return end_.first; }

    // Returns end offset of this span.
    inline int end_offset() const { return end_.second; }

private:
    const std::pair<int, int> start_;
    const std::pair<int, int> end_;
};

}  // namespace tinyc

#endif  // TINYC_SPAN_H_
