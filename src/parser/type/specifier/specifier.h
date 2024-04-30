#ifndef TINYC_PARSER_TYPE_SPECIFIER_SPECIFIER_H_
#define TINYC_PARSER_TYPE_SPECIFIER_SPECIFIER_H_

#include "../../node.h"

namespace tinyc {

enum class TypeSpecifierKind {
    Struct,
    Union,
    Enum,
};

class TypeSpecifier : public Node {
public:
    virtual ~TypeSpecifier() {}
    virtual TypeSpecifierKind kind() const = 0;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_TYPE_SPECIFIER_SPECIFIER_H_
