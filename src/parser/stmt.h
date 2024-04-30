#ifndef TINYC_PARSER_STMT_H_
#define TINYC_PARSER_STMT_H_

#include "node.h"

namespace tinyc {

enum class StatementKind {
    Labeled,
    Case,
    Default,
    Empty,
    Expression,
    Block,
    If,
    Switch,
    While,
    DoWhile,
    For,
    Goto,
    Continue,
    Break,
    Return,
};

class Statement : public Node {
public:
    virtual ~Statement() {}
    virtual StatementKind kind() const = 0;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_STMT_H_
