#ifndef TINYC_PARSER_NODE_H_
#define TINYC_PARSER_NODE_H_

#include <string>

#include "../span.h"

namespace tinyc {

// An abstract class for all element in ast.
class Node {
public:
    virtual ~Node() {}
    virtual std::string debug() const = 0;
    virtual Span span() const = 0;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_NODE_H_
