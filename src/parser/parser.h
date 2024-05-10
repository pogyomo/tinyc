#ifndef TINYC_PARSER_PARSER_H_
#define TINYC_PARSER_PARSER_H_

#include <istream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
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

class VariableOrFunctionDeclBody : public Node {
public:
    using Variable = std::optional<VariableDeclarationName>;
    using Function = std::tuple<FunctionDeclarationName, LParen,
                                std::vector<FunctionParam>, RParen>;

    VariableOrFunctionDeclBody(const std::shared_ptr<Type> type,
                               const Variable& variable)
        : type_(type), vs_(variable) {}

    VariableOrFunctionDeclBody(const std::shared_ptr<Type> type,
                               const Function& function)
        : type_(type), vs_(function) {}

    inline bool is_variable() const { return vs_.index() == 0; }

    inline bool is_function() const { return vs_.index() == 1; }

    inline const std::shared_ptr<Type>& type() const { return type_; }

    const Variable& variable() const {
        if (!is_variable()) {
            throw std::out_of_range("tried to get `Variable` but it is not");
        } else {
            return std::get<0>(vs_);
        }
    }

    const Function& function() const {
        if (!is_function()) {
            throw std::out_of_range("tried to get `Function` but it is not");
        } else {
            return std::get<1>(vs_);
        }
    }

    Span span() const override {
        if (is_variable()) {
            auto name = variable();
            if (name.has_value()) {
                return concat_spans({type_->span(), name->span()});
            } else {
                return type_->span();
            }
        } else {
            auto [name, lparen, params, rparen] = function();
            std::vector<Span> spans;
            spans.emplace_back(type_->span());
            spans.emplace_back(name.span());
            spans.emplace_back(lparen.span());
            for (const auto& param : params) spans.emplace_back(param.span());
            spans.emplace_back(rparen.span());
            return concat_spans(spans);
        }
    }

    std::string debug() const override {
        if (is_variable()) {
            auto name = variable();
            if (name.has_value()) {
                return type_->debug() + " " + name->debug();
            } else {
                return type_->debug();
            }
        } else {
            auto [name, lparen, params, rparen] = function();
            std::stringstream ss;
            ss << type_->debug();
            ss << " " << name.debug();
            ss << lparen.debug();
            if (!params.empty()) {
                ss << params[0].debug();
                for (int i = 1; i < params.size(); i++)
                    ss << ", " << params[i].debug();
            }
            ss << rparen.debug();
            return ss.str();
        }
    }

private:
    const std::shared_ptr<Type> type_;
    const std::variant<Variable, Function> vs_;
};

std::shared_ptr<Declaration> parse_decl(TokenStream& ts);
std::shared_ptr<VariablesDeclaration> parse_var_decl(TokenStream& ts);
std::shared_ptr<FunctionDeclaration> parse_fun_decl(TokenStream& ts);

StorageClassSpecifier parse_class_specifier(TokenStream& ts);
std::pair<std::vector<StorageClassSpecifier>, std::shared_ptr<ConcreteType>>
parse_decl_concrete(TokenStream& ts);
VariableOrFunctionDeclBody parse_decl_body(TokenStream& ts,
                                           const std::shared_ptr<Type>& base);
std::shared_ptr<Type> parse_arrays(TokenStream& ts,
                                   const std::shared_ptr<Type>& base);
std::tuple<LParen, std::vector<FunctionParam>, RParen> parse_fun_params(
    TokenStream& ts);
std::shared_ptr<VariableDeclarationInit> parse_var_decl_init(TokenStream& ts);

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

// Parse given input stream and return program.
// If error happen, report error and return nullopt.
std::optional<Program> parse(Context& ctx, std::istream& is,
                             const std::string& name);

}  // namespace tinyc

#endif  // TINYC_PARSER_PARSER_H_
