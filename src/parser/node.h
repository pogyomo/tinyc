#ifndef TINYC_PARSER_NODE_H_
#define TINYC_PARSER_NODE_H_

#include "../span.h"

namespace tinyc {

class Node {
public:
    virtual ~Node() {}
    virtual Span span() const = 0;
    virtual std::string debug() const = 0;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_NODE_H_
