#ifndef TINYC_PARSER_PARSER_H_
#define TINYC_PARSER_PARSER_H_

#include <memory>

#include "../lexer/stream.h"
#include "decl.h"
#include "error.h"
#include "expr.h"
#include "node.h"
#include "stmt.h"
#include "type/quantifier.h"
#include "type/specifier/builtin.h"
#include "type/specifier/enum.h"
#include "type/specifier/specifier.h"
#include "type/specifier/struct.h"
#include "type/specifier/typedef.h"
#include "type/specifier/union.h"
#include "type/type.h"

namespace tinyc {

std::shared_ptr<TypeQuantifier> parse_type_quantifier(TokenStream& ts);
std::shared_ptr<TypeSpecifier> parse_type_specifier(TokenStream& ts);
std::shared_ptr<EnumTypeSpecifier> parse_enum(TokenStream& ts);
std::shared_ptr<UnionTypeSpecifier> parse_union(TokenStream& ts);
std::shared_ptr<StructTypeSpecifier> parse_struct(TokenStream& ts);

std::shared_ptr<Declaration> parse_decl(TokenStream& ts);
std::shared_ptr<VariableDeclaration> parse_var_decl(TokenStream& ts);
std::shared_ptr<FunctionDeclaration> parse_fun_decl(TokenStream& ts);

std::shared_ptr<Statement> parse_stmt(TokenStream& ts);

std::shared_ptr<Expression> parse_expr(TokenStream& ts);

}  // namespace tinyc

#endif  // TINYC_PARSER_PARSER_H_
