#ifndef TINYC_PARSER_PARSER_H_
#define TINYC_PARSER_PARSER_H_

#include <istream>
#include <memory>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

#include "../context.h"
#include "../lexer/stream.h"
#include "decl.h"
#include "error.h"
#include "expr.h"
#include "node.h"
#include "program.h"
#include "stmt.h"
#include "type/quantifier.h"
#include "type/specifier/enum.h"
#include "type/specifier/specifier.h"
#include "type/specifier/struct.h"
#include "type/specifier/union.h"
#include "type/type.h"

namespace tinyc {

// ==================== type parser ====================

std::shared_ptr<TypeQuantifier> parse_type_quantifier(TokenStream& ts);
std::shared_ptr<TypeSpecifier> parse_type_specifier(TokenStream& ts);
std::shared_ptr<EnumTypeSpecifier> parse_enum(TokenStream& ts);
std::shared_ptr<UnionTypeSpecifier> parse_union(TokenStream& ts);
std::shared_ptr<StructTypeSpecifier> parse_struct(TokenStream& ts);

// ==================== declaration parser ====================

std::shared_ptr<Declaration> parse_decl(TokenStream& ts);
std::shared_ptr<VariableDeclarations> parse_var_decl(TokenStream& ts);
std::shared_ptr<FunctionDeclaration> parse_fun_decl(TokenStream& ts);

StorageClassSpecifier parse_class_specifier(TokenStream& ts);
std::pair<std::vector<StorageClassSpecifier>, std::shared_ptr<ConcreteType>>
parse_decl_concrete(TokenStream& ts);
std::variant<
    std::pair<std::shared_ptr<Type>, std::optional<VariableDeclarationName>>,
    std::tuple<std::shared_ptr<Type>, FunctionDeclarationName, LParen,
               std::vector<FunctionDeclarationParam>, RParen>>
parse_decl_ptr(TokenStream& ts, std::shared_ptr<ConcreteType>& concrete);

// ==================== statement parser ====================

std::shared_ptr<Statement> parse_stmt(TokenStream& ts);
std::shared_ptr<LabeledStatement> parse_labeled_stmt(TokenStream& ts);
std::shared_ptr<CaseStatement> parse_case_stmt(TokenStream& ts);
std::shared_ptr<DefaultStatement> parse_default_stmt(TokenStream& ts);
std::shared_ptr<ExpressionStatement> parse_expr_stmt(TokenStream& ts);
std::shared_ptr<BlockStatement> parse_block_stmt(TokenStream& ts);
std::shared_ptr<IfStatement> parse_if_stmt(TokenStream& ts);
std::shared_ptr<SwitchStatement> parse_switch_stmt(TokenStream& ts);
std::shared_ptr<WhileStatement> parse_while_stmt(TokenStream& ts);
std::shared_ptr<DoWhileStatement> parse_do_while_stmt(TokenStream& ts);
std::shared_ptr<ForStatement> parse_for_stmt(TokenStream& ts);
std::shared_ptr<GotoStatement> parse_goto_stmt(TokenStream& ts);
std::shared_ptr<ContinueStatement> parse_continue_stmt(TokenStream& ts);
std::shared_ptr<BreakStatement> parse_break_stmt(TokenStream& ts);
std::shared_ptr<ReturnStatement> parse_return_stmt(TokenStream& ts);

// ==================== expression parser ====================

std::shared_ptr<Expression> parse_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_assign_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_cond_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_logical_or_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_logical_and_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_or_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_xor_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_and_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_equality_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_relative_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_shift_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_additive_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_multiplicative_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_cast_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_unary_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_postfix_expr(TokenStream& ts);
std::shared_ptr<Expression> parse_primary_expr(TokenStream& ts);

// ==================== program parser ====================

Program parse(Context& ctx, std::istream& is, const std::string& name,
              std::vector<ParseError>& es);

}  // namespace tinyc

#endif  // TINYC_PARSER_PARSER_H_
