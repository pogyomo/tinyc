#include "parser.h"

#include <memory>
#include <optional>

#include "decl.h"
#include "error.h"
#include "node.h"
#include "type/quantifier.h"
#include "type/specifier/builtin.h"
#include "type/specifier/enum.h"
#include "type/specifier/struct.h"
#include "type/specifier/typedef.h"
#include "type/specifier/union.h"

namespace tinyc {

void check(TokenStream& ts) {
    if (ts.eos()) {
        ts.retrest();
        throw ParseError("expected token after this", ts.token()->span());
    }
}

void check(TokenStream& ts, TokenKind kind, const std::string& msg) {
    check(ts);
    if (ts.token()->kind() == kind) return;
    if (ts.retrest())
        throw ParseError("expected " + msg + "after this", ts.token()->span());
    else
        throw ParseError("expected this to be " + msg, ts.token()->span());
}

// ==================== type parser ====================

std::shared_ptr<TypeQuantifier> parse_type_quantifier(TokenStream& ts) {
    check(ts);
    auto span = ts.token()->span();
    if (ts.token()->kind() == TokenKind::Const) {
        ts.advance();
        return std::make_shared<TypeQuantifier>(TypeQuantifierKind::Const,
                                                span);
    } else if (ts.token()->kind() == TokenKind::Volatile) {
        ts.advance();
        return std::make_shared<TypeQuantifier>(TypeQuantifierKind::Volatile,
                                                span);
    } else {
        throw ParseError("expected const or volatile", span);
    }
}

std::shared_ptr<TypeSpecifier> parse_type_specifier(TokenStream& ts) {
    check(ts);
    auto span = ts.token()->span();
    if (ts.token()->kind() == TokenKind::Void) {
        ts.advance();
        return std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::Void, span);
    } else if (ts.token()->kind() == TokenKind::Signed) {
        ts.advance();
        return std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::Signed, span);
    } else if (ts.token()->kind() == TokenKind::Unsigned) {
        ts.advance();
        return std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::Unsigned, span);
    } else if (ts.token()->kind() == TokenKind::Char) {
        ts.advance();
        return std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::Char, span);
    } else if (ts.token()->kind() == TokenKind::Short) {
        ts.advance();
        return std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::Short, span);
    } else if (ts.token()->kind() == TokenKind::Int) {
        ts.advance();
        return std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::Int, span);
    } else if (ts.token()->kind() == TokenKind::Long) {
        ts.advance();
        return std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::Long, span);
    } else if (ts.token()->kind() == TokenKind::Float) {
        ts.advance();
        return std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::Float, span);
    } else if (ts.token()->kind() == TokenKind::Double) {
        ts.advance();
        return std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::Double, span);
    } else if (ts.token()->kind() == TokenKind::Enum) {
        return parse_enum(ts);
    } else if (ts.token()->kind() == TokenKind::Union) {
        return parse_union(ts);
    } else if (ts.token()->kind() == TokenKind::Struct) {
        return parse_struct(ts);
    } else if (ts.token()->kind() == TokenKind::Identifier) {
        auto ident =
            std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        auto name = ident->value();
        ts.advance();
        return std::make_shared<TypedefNameTypeSpecifier>(name, span);
    } else {
        throw ParseError("expected keyword or identifier", span);
    }
}

std::shared_ptr<EnumTypeSpecifier> parse_enum(TokenStream& ts) {
    check(ts, TokenKind::Enum, "enum");
    Enum enum_kw(ts.token()->span());
    ts.advance();

    std::optional<NamedEnumTypeSpecifierName> name(std::nullopt);
    if (ts.token()->kind() == TokenKind::Identifier) {
        auto tk = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        name.emplace(NamedEnumTypeSpecifierName(tk->value(), tk->span()));
        ts.advance();
    }

    check(ts);
    if (ts.token()->kind() == TokenKind::LCurly) {
        LCurly lcurly(ts.token()->span());
        ts.advance();

        std::vector<Enumerator> enumerators;
        while (true) {
            check(ts);
            if (ts.token()->kind() == TokenKind::RCurly) {
                RCurly rcurly(ts.token()->span());
                ts.advance();

                EnumTypeSpecifierBody body(lcurly, enumerators, rcurly);
                if (name.has_value()) {
                    return std::make_shared<NamedEnumTypeSpecifier>(
                        enum_kw, name.value(), body);
                } else {
                    return std::make_shared<AnonymousEnumTypeSpecifier>(enum_kw,
                                                                        body);
                }
            }

            check(ts, TokenKind::Identifier, "identifier");
            auto tk =
                std::static_pointer_cast<ValueToken<std::string>>(ts.token());
            EnumeratorIdent ident(tk->value(), tk->span());
            ts.advance();

            check(ts);
            std::optional<EnumeratorInit> init(std::nullopt);
            if (ts.token()->kind() == TokenKind::Assign) {
                Assign assign(ts.token()->span());
                ts.advance();
                init.emplace(assign, parse_expr(ts));
            }

            check(ts);
            if (ts.token()->kind() == TokenKind::Comma) {
                Comma comma(ts.token()->span());
                if (init.has_value()) {
                    enumerators.emplace_back(
                        Enumerator(ident, init.value(), comma));
                } else {
                    enumerators.emplace_back(Enumerator(ident, comma));
                }
            } else if (ts.token()->kind() == TokenKind::RCurly) {
                continue;
            } else {
                throw ParseError("expected , or } after enumerator",
                                 ts.token()->span());
            }
        }
    } else if (name.has_value()) {
        return std::make_shared<NamedEnumTypeSpecifier>(enum_kw, name.value());
    } else {
        throw ParseError("expected either block or identifier after `enum`",
                         ts.token()->span());
    }
}

std::shared_ptr<UnionTypeSpecifier> parse_union(TokenStream& ts) {
    check(ts, TokenKind::Union, "union");
    Union union_kw(ts.token()->span());
    ts.advance();

    std::optional<NamedUnionTypeSpecifierName> name(std::nullopt);
    if (ts.token()->kind() == TokenKind::Identifier) {
        auto tk = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        name.emplace(NamedUnionTypeSpecifierName(tk->value(), tk->span()));
        ts.advance();
    }

    check(ts);
    if (ts.token()->kind() == TokenKind::LCurly) {
        LCurly lcurly(ts.token()->span());
        ts.advance();

        std::vector<VariableDeclaration> decls;
        while (true) {
            check(ts);
            if (ts.token()->kind() == TokenKind::RCurly) {
                RCurly rcurly(ts.token()->span());
                ts.advance();

                UnionTypeSpecifierBody body(lcurly, decls, rcurly);
                if (name.has_value()) {
                    return std::make_shared<NamedUnionTypeSpecifier>(
                        union_kw, name.value(), body);
                } else {
                    return std::make_shared<AnonymousUnionTypeSpecifier>(
                        union_kw, body);
                }
            }
            decls.emplace_back(*parse_var_decl(ts));
        }
    } else if (name.has_value()) {
        return std::make_shared<NamedUnionTypeSpecifier>(union_kw,
                                                         name.value());
    } else {
        throw ParseError("expected either block or identifier after `union`",
                         ts.token()->span());
    }
}

std::shared_ptr<StructTypeSpecifier> parse_struct(TokenStream& ts) {
    check(ts, TokenKind::Struct, "struct");
    Struct struct_kw(ts.token()->span());
    ts.advance();

    std::optional<NamedStructTypeSpecifierName> name(std::nullopt);
    if (ts.token()->kind() == TokenKind::Identifier) {
        auto tk = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        name.emplace(NamedStructTypeSpecifierName(tk->value(), tk->span()));
        ts.advance();
    }

    check(ts);
    if (ts.token()->kind() == TokenKind::LCurly) {
        LCurly lcurly(ts.token()->span());
        ts.advance();

        std::vector<VariableDeclaration> decls;
        while (true) {
            check(ts);
            if (ts.token()->kind() == TokenKind::RCurly) {
                RCurly rcurly(ts.token()->span());
                ts.advance();

                StructTypeSpecifierBody body(lcurly, decls, rcurly);
                if (name.has_value()) {
                    return std::make_shared<NamedStructTypeSpecifier>(
                        struct_kw, name.value(), body);
                } else {
                    return std::make_shared<AnonymousStructTypeSpecifier>(
                        struct_kw, body);
                }
            }
            decls.emplace_back(*parse_var_decl(ts));
        }
    } else if (name.has_value()) {
        return std::make_shared<NamedStructTypeSpecifier>(struct_kw,
                                                          name.value());
    } else {
        throw ParseError("expected either block or identifier after `struct`",
                         ts.token()->span());
    }
}

// ==================== declaration parser ====================

std::shared_ptr<Declaration> parse_decl(TokenStream& ts) {}

std::shared_ptr<VariableDeclaration> parse_var_decl(TokenStream& ts) {}

std::shared_ptr<FunctionDeclaration> parse_fun_decl(TokenStream& ts) {}

// ==================== statement parser ====================

std::shared_ptr<Statement> parse_stmt(TokenStream& ts) {}

// ==================== expression parser ====================

std::shared_ptr<Expression> parse_expr(TokenStream& ts) {}

}  // namespace tinyc
