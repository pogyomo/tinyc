#ifndef TINYC_SPAN_H_
#define TINYC_SPAN_H_

#include <utility>
#include <vector>

namespace tinyc {

// A class represent the position in source code.
class Position {
public:
    Position() : row_(0), offset_(0) {}

    Position(int row, int offset) : row_(row), offset_(offset) {}

    int row() const { return row_; }

    int offset() const { return offset_; }

    bool operator<(const Position& rhs) const {
        return std::make_pair(row_, offset_) <
               std::make_pair(rhs.row_, rhs.offset_);
    }

    bool operator>(const Position& rhs) const {
        return std::make_pair(row_, offset_) >
               std::make_pair(rhs.row_, rhs.offset_);
    }

    bool operator<=(const Position& rhs) const { return !(*this > rhs); }

    bool operator>=(const Position& rhs) const { return !(*this < rhs); }

private:
    int row_;
    int offset_;
};

// A non-empty, inclusive range in soruce code.
class Span {
public:
    Span(int id) : id_(id), start_(), end_() {}

    Span(int id, Position start, Position end)
        : id_(id), start_(start), end_(end) {}

    // Returns a unique id which represent the source code this span was
    // created.
    int id() const { return id_; }

    // Returns a start position of this range.
    Position start() const { return start_; }

    // Returns a end position of this range.
    Position end() const { return end_; }

private:
    int id_;
    Position start_;
    Position end_;
};

// Combine given spans into one span.
Span concat_spans(const std::vector<Span>& spans);

}  // namespace tinyc

#endif  // TINYC_SPAN_H_
