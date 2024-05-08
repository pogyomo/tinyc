#include "parser.h"

#include <iostream>
#include <istream>
#include <memory>
#include <optional>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

#include "../lexer/lexer.h"
#include "../preprocessor/preprocessor.h"
#include "decl.h"
#include "error.h"
#include "node.h"
#include "stmt.h"
#include "type/quantifier.h"
#include "type/specifier/builtin.h"
#include "type/specifier/enum.h"
#include "type/specifier/struct.h"
#include "type/specifier/typedef.h"
#include "type/specifier/union.h"
#include "type/type.h"

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
        throw ParseError("expected " + msg + " after this", ts.token()->span());
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
                ts.advance();
                if (init.has_value()) {
                    enumerators.emplace_back(
                        Enumerator(ident, init.value(), comma));
                } else {
                    enumerators.emplace_back(Enumerator(ident, comma));
                }
            } else if (ts.token()->kind() == TokenKind::RCurly) {
                if (init.has_value()) {
                    enumerators.emplace_back(Enumerator(ident, init.value()));
                } else {
                    enumerators.emplace_back(Enumerator(ident));
                }
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

        std::vector<VariableDeclarations> decls;
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
            auto decl = parse_var_decl(ts);
            for (const auto& d : decl->decls()) {
                if (!d->class_specifiers().empty()) {
                    throw ParseError(
                        "class specifier in union member is not allowed",
                        d->span());
                }
            }
            decls.emplace_back(*decl);
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

        std::vector<VariableDeclarations> decls;
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
            auto decl = parse_var_decl(ts);
            for (const auto& d : decl->decls()) {
                if (!d->class_specifiers().empty()) {
                    throw ParseError(
                        "class specifier in struct member is not allowed",
                        d->span());
                }
            }
            decls.emplace_back(*decl);
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

// FIXME: This implementation is broken:
//        * can't distinguish int *a[10]; and int (*a)[10];
//        * can't parse int (*a)(int, int);
//        * can't parse int a = 1, b = 2; and recognize it as
//        int a = (1, b = 2);
std::shared_ptr<Declaration> parse_decl(TokenStream& ts) {
    auto state = ts.get_state();
    try {
        return parse_fun_decl(ts);
    } catch (ParseError e) {
        // TODO: Don't catch error when parser parse ty ident(args) correctly
        //       and failed to parse its body.
        ts.set_state(state);
        return parse_var_decl(ts);
    }
}

StorageClassSpecifier parse_class_specifier(TokenStream& ts) {
    check(ts);
    auto kind = ts.token()->kind();
    auto span = ts.token()->span();
    if (kind == TokenKind::Auto) {
        ts.advance();
        return StorageClassSpecifier(StorageClassSpecifierKind::Auto, span);
    } else if (kind == TokenKind::Register) {
        ts.advance();
        return StorageClassSpecifier(StorageClassSpecifierKind::Register, span);
    } else if (kind == TokenKind::Static) {
        ts.advance();
        return StorageClassSpecifier(StorageClassSpecifierKind::Static, span);
    } else if (kind == TokenKind::Extern) {
        ts.advance();
        return StorageClassSpecifier(StorageClassSpecifierKind::Extern, span);
    } else if (kind == TokenKind::Typedef) {
        ts.advance();
        return StorageClassSpecifier(StorageClassSpecifierKind::Typedef, span);
    } else {
        std::stringstream ss;
        ss << "expected this to be one of ";
        ss << "auto, register, static, extern or typedef";
        throw ParseError(ss.str(), span);
    }
}

std::shared_ptr<VariableDeclarations> parse_var_decl(TokenStream& ts) {
    auto [cs, concrete] = parse_var_decl_concrete(ts);

    std::vector<std::shared_ptr<VariableDeclaration>> decls;
    while (true) {
        auto [type, name] = parse_var_decl_ptr(ts, concrete);

        if (name.has_value() && !ts.eos() &&
            ts.token()->kind() == TokenKind::Assign) {
            Assign assign(ts.token()->span());
            ts.advance();
            auto expr = parse_assign_expr(ts);
            VariableDeclarationInitializer initializer(assign, expr);

            decls.emplace_back(std::make_shared<NamedVariableDeclaration>(
                cs, type, name.value(), initializer));
        } else {
            if (name.has_value())
                decls.emplace_back(std::make_shared<NamedVariableDeclaration>(
                    cs, type, name.value()));
            else
                decls.emplace_back(
                    std::make_shared<AnonymousVariableDeclaration>(cs, type));
        }

        if (!ts.eos() && ts.token()->kind() == TokenKind::Comma) {
            ts.advance();
        } else if (!ts.eos() && ts.token()->kind() == TokenKind::Semicolon) {
            Semicolon semicolon(ts.token()->span());
            ts.advance();
            return std::make_shared<VariableDeclarations>(decls, semicolon);
        } else {
            ts.retrest();
            throw ParseError("expected , or ; after this", ts.token()->span());
        }
    }
}

std::pair<std::vector<StorageClassSpecifier>, std::shared_ptr<ConcreteType>>
parse_var_decl_concrete(TokenStream& ts) {
    std::vector<StorageClassSpecifier> class_specifiers;
    std::vector<std::shared_ptr<TypeSpecifier>> specifiers;
    std::vector<std::shared_ptr<TypeQuantifier>> quantifiers;
    while (true) {
        check(ts);
        auto kind = ts.token()->kind();
        if (kind == TokenKind::Void || kind == TokenKind::Signed ||
            kind == TokenKind::Unsigned || kind == TokenKind::Char ||
            kind == TokenKind::Short || kind == TokenKind::Int ||
            kind == TokenKind::Long || kind == TokenKind::Float ||
            kind == TokenKind::Double || kind == TokenKind::Enum ||
            kind == TokenKind::Union || kind == TokenKind::Struct) {
            specifiers.emplace_back(parse_type_specifier(ts));
        } else if (kind == TokenKind::Const || kind == TokenKind::Volatile) {
            quantifiers.emplace_back(parse_type_quantifier(ts));
        } else if (kind == TokenKind::Auto || kind == TokenKind::Register ||
                   kind == TokenKind::Static || kind == TokenKind::Extern ||
                   kind == TokenKind::Typedef) {
            class_specifiers.emplace_back(parse_class_specifier(ts));
        } else if (ts.token()->kind() == TokenKind::Identifier) {
            ts.advance();
            check(ts);
            if (ts.token()->kind() == TokenKind::Semicolon ||
                ts.token()->kind() == TokenKind::Assign ||
                ts.token()->kind() == TokenKind::Comma) {
                ts.retrest();
                if (specifiers.empty()) {
                    throw ParseError("no specifiers found", ts.token()->span());
                }
                auto concrete =
                    std::make_shared<ConcreteType>(specifiers, quantifiers);
                return {class_specifiers, concrete};
            } else {
                ts.retrest();
                specifiers.emplace_back(parse_type_specifier(ts));
            }
        } else {
            if (specifiers.empty()) {
                throw ParseError("no specifiers found", ts.token()->span());
            }
            auto concrete =
                std::make_shared<ConcreteType>(specifiers, quantifiers);
            return {class_specifiers, concrete};
        }
    }
}

// TODO: parse more complex type like a[10], (*a)[10], (*a)(int, float)
std::pair<std::shared_ptr<Type>, std::optional<VariableDeclarationName>>
parse_var_decl_ptr(TokenStream& ts, std::shared_ptr<ConcreteType>& concrete) {
    std::shared_ptr<Type> type = concrete;

    while (true) {
        if (!ts.eos() && ts.token()->kind() == TokenKind::Star) {
            Star star(ts.token()->span());
            ts.advance();

            std::vector<std::shared_ptr<TypeQuantifier>> quantifiers;
            while (true) {
                if (!ts.eos() && (ts.token()->kind() == TokenKind::Const ||
                                  ts.token()->kind() == TokenKind::Volatile)) {
                    quantifiers.emplace_back(parse_type_quantifier(ts));
                } else {
                    break;
                }
            }

            type = std::make_shared<PointerType>(star, quantifiers, type);
        } else {
            break;
        }
    }

    std::optional<VariableDeclarationName> name;
    if (!ts.eos() && ts.token()->kind() == TokenKind::Identifier) {
        auto id = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        name.emplace(id->value(), id->span());
        ts.advance();
    }

    return {type, name};
}

std::shared_ptr<FunctionDeclaration> parse_fun_decl(TokenStream& ts) {
    auto [cs, concrete] = parse_fun_decl_concrete(ts);
    auto [ret_type, name, lparen, args, rparen] =
        parse_fun_decl_ptr(ts, concrete);

    check(ts);
    if (ts.token()->kind() == TokenKind::Semicolon) {
        Semicolon semicolon(ts.token()->span());
        ts.advance();
        return std::make_shared<FunctionDeclaration>(cs, ret_type, name, lparen,
                                                     args, rparen, semicolon);
    } else if (ts.token()->kind() == TokenKind::LCurly) {
        auto body = parse_block_stmt(ts);
        if (!ts.eos() && ts.token()->kind() == TokenKind::Semicolon) {
            Semicolon semicolon(ts.token()->span());
            ts.advance();
            return std::make_shared<FunctionDeclaration>(
                cs, ret_type, name, lparen, args, rparen, body, semicolon);
        } else {
            return std::make_shared<FunctionDeclaration>(
                cs, ret_type, name, lparen, args, rparen, body);
        }
    } else {
        ts.retrest();
        throw ParseError("expected ; or { after this", ts.token()->span());
    }
}

std::pair<std::vector<StorageClassSpecifier>, std::shared_ptr<ConcreteType>>
parse_fun_decl_concrete(TokenStream& ts) {
    std::vector<StorageClassSpecifier> class_specifiers;
    std::vector<std::shared_ptr<TypeSpecifier>> specifiers;
    std::vector<std::shared_ptr<TypeQuantifier>> quantifiers;
    while (true) {
        check(ts);
        auto kind = ts.token()->kind();
        if (kind == TokenKind::Void || kind == TokenKind::Signed ||
            kind == TokenKind::Unsigned || kind == TokenKind::Char ||
            kind == TokenKind::Short || kind == TokenKind::Int ||
            kind == TokenKind::Long || kind == TokenKind::Float ||
            kind == TokenKind::Double || kind == TokenKind::Enum ||
            kind == TokenKind::Union || kind == TokenKind::Struct) {
            specifiers.emplace_back(parse_type_specifier(ts));
        } else if (kind == TokenKind::Const || kind == TokenKind::Volatile) {
            quantifiers.emplace_back(parse_type_quantifier(ts));
        } else if (kind == TokenKind::Auto || kind == TokenKind::Register ||
                   kind == TokenKind::Static || kind == TokenKind::Extern ||
                   kind == TokenKind::Typedef) {
            class_specifiers.emplace_back(parse_class_specifier(ts));
        } else if (ts.token()->kind() == TokenKind::Identifier) {
            ts.advance();
            check(ts);
            if (ts.token()->kind() == TokenKind::LParen) {
                ts.retrest();
                if (specifiers.empty()) {
                    throw ParseError("no specifiers found", ts.token()->span());
                }
                auto concrete =
                    std::make_shared<ConcreteType>(specifiers, quantifiers);
                return {class_specifiers, concrete};
            } else {
                ts.retrest();
                specifiers.emplace_back(parse_type_specifier(ts));
            }
        } else {
            if (specifiers.empty()) {
                throw ParseError("no specifiers found", ts.token()->span());
            }
            auto concrete =
                std::make_shared<ConcreteType>(specifiers, quantifiers);
            return {class_specifiers, concrete};
        }
    }
}

// TODO: parse more complex type like (*a(int, int))(int, int)
std::tuple<std::shared_ptr<Type>, FunctionDeclarationName, LParen,
           std::vector<FunctionDeclarationArg>, RParen>
parse_fun_decl_ptr(TokenStream& ts, std::shared_ptr<ConcreteType>& concrete) {
    std::shared_ptr<Type> type = concrete;

    while (true) {
        if (!ts.eos() && ts.token()->kind() == TokenKind::Star) {
            Star star(ts.token()->span());
            ts.advance();

            std::vector<std::shared_ptr<TypeQuantifier>> quantifiers;
            while (true) {
                if (!ts.eos() && (ts.token()->kind() == TokenKind::Const ||
                                  ts.token()->kind() == TokenKind::Volatile)) {
                    quantifiers.emplace_back(parse_type_quantifier(ts));
                } else {
                    break;
                }
            }

            type = std::make_shared<PointerType>(star, quantifiers, type);
        } else {
            break;
        }
    }

    check(ts, TokenKind::Identifier, "identifier");
    auto id = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
    FunctionDeclarationName name(id->value(), id->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    LParen lparen(ts.token()->span());
    ts.advance();

    std::vector<FunctionDeclarationArg> args;
    while (true) {
        if (!ts.eos() && ts.token()->kind() == TokenKind::RParen) {
            RParen rparen(ts.token()->span());
            ts.advance();
            return {type, name, lparen, args, rparen};
        }

        auto [cs, concrete_] = parse_var_decl_concrete(ts);
        auto [type_, name_] = parse_var_decl_ptr(ts, concrete_);
        if (name_.has_value())
            args.emplace_back(type_, FunctionDeclarationArgName(name_->name(),
                                                                name_->span()));
        else
            args.emplace_back(type_);

        if (!ts.eos() && ts.token()->kind() == TokenKind::RParen) {
            continue;
        } else if (!ts.eos() && ts.token()->kind() == TokenKind::Comma) {
            ts.advance();
        } else {
            ts.retrest();
            throw ParseError("expected ) or , after this", ts.token()->span());
        }
    }
}

// ==================== statement parser ====================

std::shared_ptr<Statement> parse_stmt(TokenStream& ts) {
    check(ts);
    if (ts.token()->kind() == TokenKind::Identifier) {
        ts.advance();
        if (!ts.eos() && ts.token()->kind() == TokenKind::Colon) {
            return parse_labeled_stmt(ts);
        } else {
            ts.retrest();
            return parse_expr_stmt(ts);
        }
    } else if (ts.token()->kind() == TokenKind::Case) {
        return parse_case_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::Default) {
        return parse_default_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::LCurly) {
        return parse_block_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::If) {
        return parse_if_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::Switch) {
        return parse_switch_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::While) {
        return parse_while_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::Do) {
        return parse_do_while_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::For) {
        return parse_for_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::Goto) {
        return parse_goto_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::Break) {
        return parse_break_stmt(ts);
    } else if (ts.token()->kind() == TokenKind::Return) {
        return parse_return_stmt(ts);
    } else {
        return parse_expr_stmt(ts);
    }
}

std::shared_ptr<LabeledStatement> parse_labeled_stmt(TokenStream& ts) {
    check(ts, TokenKind::Identifier, "identifier");
    auto tk = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
    LabeledStatementLabel label(tk->value(), tk->span());
    ts.advance();

    check(ts, TokenKind::Colon, ":");
    Colon colon(ts.token()->span());
    ts.advance();

    auto stmt = parse_stmt(ts);

    return std::make_shared<LabeledStatement>(label, colon, stmt);
}

std::shared_ptr<CaseStatement> parse_case_stmt(TokenStream& ts) {
    check(ts, TokenKind::Case, "case");
    Case case_kw(ts.token()->span());
    ts.advance();

    auto expr = parse_expr(ts);

    check(ts, TokenKind::Colon, ":");
    Colon colon(ts.token()->span());
    ts.advance();

    auto stmt = parse_stmt(ts);

    return std::make_shared<CaseStatement>(case_kw, expr, colon, stmt);
}

std::shared_ptr<DefaultStatement> parse_default_stmt(TokenStream& ts) {
    check(ts, TokenKind::Default, "default");
    Default default_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Colon, ":");
    Colon colon(ts.token()->span());
    ts.advance();

    auto stmt = parse_stmt(ts);

    return std::make_shared<DefaultStatement>(default_kw, colon, stmt);
}

std::shared_ptr<BlockStatement> parse_block_stmt(TokenStream& ts) {
    check(ts, TokenKind::LCurly, "{");
    LCurly lcurly(ts.token()->span());
    ts.advance();

    std::vector<std::shared_ptr<BlockStatementItem>> items;
    while (true) {
        if (ts.token()->kind() == TokenKind::RCurly) {
            RCurly rcurly(ts.token()->span());
            ts.advance();
            return std::make_shared<BlockStatement>(lcurly, items, rcurly);
        }

        try {
            auto stmt = parse_stmt(ts);
            items.push_back(
                std::make_shared<BlockStatementStatementItem>(stmt));
        } catch (ParseError e) {
            auto decl = parse_decl(ts);
            items.push_back(
                std::make_shared<BlockStatementDeclarationItem>(decl));
        }
    }
}

std::shared_ptr<IfStatement> parse_if_stmt(TokenStream& ts) {
    check(ts, TokenKind::If, "if");
    If if_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    LParen lparen(ts.token()->span());
    ts.advance();

    auto cond = parse_expr(ts);

    check(ts, TokenKind::RParen, ")");
    RParen rparen(ts.token()->span());
    ts.advance();

    auto then = parse_stmt(ts);

    IfStatementIfBody if_body(if_kw, lparen, cond, rparen, then);

    if (ts.eos() || ts.token()->kind() != TokenKind::Else) {
        return std::make_shared<IfStatement>(if_body);
    }

    Else else_kw(ts.token()->span());
    ts.advance();

    auto otherwise = parse_stmt(ts);

    IfStatementElseBody else_body = IfStatementElseBody(else_kw, otherwise);

    return std::make_shared<IfStatement>(if_body, else_body);
}

std::shared_ptr<SwitchStatement> parse_switch_stmt(TokenStream& ts) {
    check(ts, TokenKind::Switch, "switch");
    Switch switch_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    LParen lparen(ts.token()->span());
    ts.advance();

    auto comp = parse_expr(ts);

    check(ts, TokenKind::RParen, ")");
    RParen rparen(ts.token()->span());
    ts.advance();

    auto body = parse_stmt(ts);

    return std::make_shared<SwitchStatement>(switch_kw, lparen, comp, rparen,
                                             body);
}

std::shared_ptr<WhileStatement> parse_while_stmt(TokenStream& ts) {
    check(ts, TokenKind::While, "while");
    While switch_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    LParen lparen(ts.token()->span());
    ts.advance();

    auto cond = parse_expr(ts);

    check(ts, TokenKind::RParen, ")");
    RParen rparen(ts.token()->span());
    ts.advance();

    auto body = parse_stmt(ts);

    return std::make_shared<WhileStatement>(switch_kw, lparen, cond, rparen,
                                            body);
}

std::shared_ptr<DoWhileStatement> parse_do_while_stmt(TokenStream& ts) {
    check(ts, TokenKind::Do, "do");
    Do do_kw(ts.token()->span());
    ts.advance();

    auto body = parse_stmt(ts);

    check(ts, TokenKind::While, "while");
    While while_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    LParen lparen(ts.token()->span());
    ts.advance();

    auto cond = parse_expr(ts);

    check(ts, TokenKind::RParen, ")");
    RParen rparen(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Semicolon, ";");
    Semicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<DoWhileStatement>(do_kw, body, while_kw, lparen,
                                              cond, rparen, semicolon);
}

std::shared_ptr<ForStatement> parse_for_stmt(TokenStream& ts) {
    check(ts, TokenKind::For, "for");
    For for_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    LParen lparen(ts.token()->span());
    ts.advance();

    std::optional<ForStatementInit> init = std::nullopt;
    check(ts);
    if (ts.token()->kind() != TokenKind::Semicolon) {
        auto expr = parse_expr(ts);

        check(ts, TokenKind::Semicolon, ";");
        Semicolon semicolon(ts.token()->span());
        ts.advance();

        init.emplace(expr, semicolon);
    } else {
        check(ts, TokenKind::Semicolon, ";");
        Semicolon semicolon(ts.token()->span());
        ts.advance();

        init.emplace(semicolon);
    }

    std::optional<ForStatementCond> cond = std::nullopt;
    check(ts);
    if (ts.token()->kind() != TokenKind::Semicolon) {
        auto expr = parse_expr(ts);

        check(ts, TokenKind::Semicolon, ";");
        Semicolon semicolon(ts.token()->span());
        ts.advance();

        cond.emplace(expr, semicolon);
    } else {
        check(ts, TokenKind::Semicolon, ";");
        Semicolon semicolon(ts.token()->span());
        ts.advance();

        cond.emplace(semicolon);
    }

    std::optional<std::shared_ptr<Expression>> update = std::nullopt;
    check(ts);
    if (ts.token()->kind() != TokenKind::RParen) {
        update = parse_expr(ts);
    }
    check(ts, TokenKind::RParen, ")");
    RParen rparen(ts.token()->span());
    ts.advance();

    auto body = parse_stmt(ts);

    if (update.has_value()) {
        return std::make_shared<ForStatement>(for_kw, lparen, init.value(),
                                              cond.value(), update.value(),
                                              rparen, body);
    } else {
        return std::make_shared<ForStatement>(for_kw, lparen, init.value(),
                                              cond.value(), rparen, body);
    }
}

std::shared_ptr<GotoStatement> parse_goto_stmt(TokenStream& ts) {
    check(ts, TokenKind::Goto, "goto");
    Goto goto_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Identifier, "identifier");
    auto tk = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
    GotoStatementLabel label(tk->value(), tk->span());
    ts.advance();

    check(ts, TokenKind::Semicolon, ";");
    Semicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<GotoStatement>(goto_kw, label, semicolon);
}

std::shared_ptr<ContinueStatement> parse_continue_stmt(TokenStream& ts) {
    check(ts, TokenKind::Continue, "continue");
    Continue continue_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Semicolon, ";");
    Semicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<ContinueStatement>(continue_kw, semicolon);
}

std::shared_ptr<BreakStatement> parse_break_stmt(TokenStream& ts) {
    check(ts, TokenKind::Break, "break");
    Break break_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Semicolon, ";");
    Semicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<BreakStatement>(break_kw, semicolon);
}

std::shared_ptr<ReturnStatement> parse_return_stmt(TokenStream& ts) {
    check(ts, TokenKind::Return, "return");
    Return return_kw(ts.token()->span());
    ts.advance();

    auto ret = parse_expr(ts);

    check(ts, TokenKind::Semicolon, ";");
    Semicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<ReturnStatement>(return_kw, ret, semicolon);
}

std::shared_ptr<ExpressionStatement> parse_expr_stmt(TokenStream& ts) {
    check(ts);
    if (ts.token()->kind() == TokenKind::Semicolon) {
        Semicolon semicolon(ts.token()->span());
        ts.advance();
        return std::make_shared<ExpressionStatement>(semicolon);
    } else {
        auto expr = parse_expr(ts);

        check(ts, TokenKind::Semicolon, ";");
        Semicolon semicolon(ts.token()->span());
        ts.advance();

        return std::make_shared<ExpressionStatement>(expr, semicolon);
    }
}

// ==================== expression parser ====================

std::shared_ptr<Expression> parse_expr(TokenStream& ts) {
    auto lhs = parse_assign_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Comma) {
            InfixExpressionOp op(InfixExpressionOpKind::Comma,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_assign_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_assign_expr(TokenStream& ts) {
    auto state = ts.get_state();

    std::shared_ptr<Expression> lhs;
    InfixExpressionOpKind kind;
    try {
        lhs = parse_unary_expr(ts);
        check(ts);
        switch (ts.token()->kind()) {
            case TokenKind::Assign:
                kind = InfixExpressionOpKind::Assign;
                break;
            case TokenKind::OrAssign:
                kind = InfixExpressionOpKind::OrAssign;
                break;
            case TokenKind::XorAssign:
                kind = InfixExpressionOpKind::XorAssign;
                break;
            case TokenKind::AndAssign:
                kind = InfixExpressionOpKind::AndAssign;
                break;
            case TokenKind::LShiftAssign:
                kind = InfixExpressionOpKind::LShiftAssign;
                break;
            case TokenKind::RShiftAssign:
                kind = InfixExpressionOpKind::RShiftAssign;
                break;
            case TokenKind::AddAssign:
                kind = InfixExpressionOpKind::AddAssign;
                break;
            case TokenKind::SubAssign:
                kind = InfixExpressionOpKind::SubAssign;
                break;
            case TokenKind::MulAssign:
                kind = InfixExpressionOpKind::MulAssign;
                break;
            case TokenKind::DivAssign:
                kind = InfixExpressionOpKind::DivAssign;
                break;
            case TokenKind::ModAssign:
                kind = InfixExpressionOpKind::ModAssign;
                break;
            default:
                throw ParseError("expected assign operator",
                                 ts.token()->span());
        }
    } catch (ParseError e) {
        ts.set_state(state);
        return parse_cond_expr(ts);
    }

    InfixExpressionOp op(kind, ts.token()->span());
    ts.advance();

    auto rhs = parse_assign_expr(ts);
    return std::make_shared<InfixExpression>(lhs, op, rhs);
}

std::shared_ptr<Expression> parse_cond_expr(TokenStream& ts) {
    auto cond = parse_logical_or_expr(ts);

    if (ts.eos() || ts.token()->kind() != TokenKind::Question) {
        return cond;
    }
    auto exclamation = Exclamation(ts.token()->span());
    ts.advance();

    auto then = parse_expr(ts);

    check(ts, TokenKind::Colon, ":");
    auto colon = Colon(ts.token()->span());
    ts.advance();

    auto otherwise = parse_cond_expr(ts);
    return std::make_shared<ConditionalExpression>(cond, exclamation, then,
                                                   colon, otherwise);
}

std::shared_ptr<Expression> parse_logical_or_expr(TokenStream& ts) {
    auto lhs = parse_logical_and_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Or) {
            InfixExpressionOp op(InfixExpressionOpKind::Or, ts.token()->span());
            ts.advance();
            auto rhs = parse_logical_and_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_logical_and_expr(TokenStream& ts) {
    auto lhs = parse_or_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::And) {
            InfixExpressionOp op(InfixExpressionOpKind::And,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_or_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_or_expr(TokenStream& ts) {
    auto lhs = parse_xor_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Vertical) {
            InfixExpressionOp op(InfixExpressionOpKind::BitOr,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_xor_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_xor_expr(TokenStream& ts) {
    auto lhs = parse_and_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Hat) {
            InfixExpressionOp op(InfixExpressionOpKind::BitXor,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_and_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_and_expr(TokenStream& ts) {
    auto lhs = parse_equality_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Ampersand) {
            InfixExpressionOp op(InfixExpressionOpKind::BitAnd,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_equality_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_equality_expr(TokenStream& ts) {
    auto lhs = parse_relative_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::EQ) {
            InfixExpressionOp op(InfixExpressionOpKind::EQ, ts.token()->span());
            ts.advance();
            auto rhs = parse_relative_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::NE) {
            InfixExpressionOp op(InfixExpressionOpKind::NE, ts.token()->span());
            ts.advance();
            auto rhs = parse_relative_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_relative_expr(TokenStream& ts) {
    auto lhs = parse_shift_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::LT) {
            InfixExpressionOp op(InfixExpressionOpKind::LT, ts.token()->span());
            ts.advance();
            auto rhs = parse_shift_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::GT) {
            InfixExpressionOp op(InfixExpressionOpKind::GT, ts.token()->span());
            ts.advance();
            auto rhs = parse_shift_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::LE) {
            InfixExpressionOp op(InfixExpressionOpKind::LE, ts.token()->span());
            ts.advance();
            auto rhs = parse_shift_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::GE) {
            InfixExpressionOp op(InfixExpressionOpKind::GE, ts.token()->span());
            ts.advance();
            auto rhs = parse_shift_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_shift_expr(TokenStream& ts) {
    auto lhs = parse_additive_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::LShift) {
            InfixExpressionOp op(InfixExpressionOpKind::LShift,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_additive_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::RShift) {
            InfixExpressionOp op(InfixExpressionOpKind::RShift,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_additive_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_additive_expr(TokenStream& ts) {
    auto lhs = parse_multiplicative_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Plus) {
            InfixExpressionOp op(InfixExpressionOpKind::Add,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_multiplicative_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::Minus) {
            InfixExpressionOp op(InfixExpressionOpKind::Sub,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_multiplicative_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_multiplicative_expr(TokenStream& ts) {
    auto lhs = parse_cast_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Star) {
            InfixExpressionOp op(InfixExpressionOpKind::Mul,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_cast_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::Slash) {
            InfixExpressionOp op(InfixExpressionOpKind::Div,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_cast_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::Percent) {
            InfixExpressionOp op(InfixExpressionOpKind::Mod,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_cast_expr(ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_cast_expr(TokenStream& ts) {
    check(ts);
    auto state = ts.get_state();
    try {
        if (ts.token()->kind() != TokenKind::LParen) {
            return parse_unary_expr(ts);
        }
        LParen lparen(ts.token()->span());
        ts.advance();

        auto [cs, concrete] = parse_var_decl_concrete(ts);
        auto [type, name] = parse_var_decl_ptr(ts, concrete);

        check(ts, TokenKind::RParen, ")");
        RParen rparen(ts.token()->span());
        ts.advance();

        auto expr = parse_unary_expr(ts);

        return std::make_shared<CastExpression>(lparen, type, rparen, expr);
    } catch (ParseError e) {
        ts.set_state(state);
        return parse_unary_expr(ts);
    }
}

std::shared_ptr<Expression> parse_unary_expr(TokenStream& ts) {
    check(ts);
    if (ts.token()->kind() == TokenKind::PlusPlus) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Inc, ts.token()->span());
        ts.advance();
        auto expr = parse_unary_expr(ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::MinusMinus) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Dec, ts.token()->span());
        ts.advance();
        auto expr = parse_unary_expr(ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Ampersand) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Ref, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Star) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Deref, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Plus) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Plus, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Minus) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Minus, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Tilde) {
        UnaryExpressionOp op(UnaryExpressionOpKind::BitInv, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Not) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Neg, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Sizeof) {
        Sizeof sizeof_kw(ts.token()->span());
        ts.advance();
        auto state = ts.get_state();
        try {
            auto expr = parse_unary_expr(ts);
            return std::make_shared<SizeofExprExpression>(sizeof_kw, expr);
        } catch (ParseError e) {
            ts.set_state(state);

            check(ts, TokenKind::LParen, "(");
            LParen lparen(ts.token()->span());
            ts.advance();

            auto [cs, concrete] = parse_var_decl_concrete(ts);
            auto [type, name] = parse_var_decl_ptr(ts, concrete);

            check(ts, TokenKind::RParen, ")");
            RParen rparen(ts.token()->span());
            ts.advance();

            return std::make_shared<SizeofTypeExpression>(sizeof_kw, lparen,
                                                          type, rparen);
        }
    } else {
        return parse_postfix_expr(ts);
    }
}

std::shared_ptr<Expression> parse_postfix_expr(TokenStream& ts) {
    auto lhs = parse_primary_expr(ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::LSquare) {
            LSquare lsquare(ts.token()->span());
            ts.advance();

            auto index = parse_expr(ts);

            check(ts, TokenKind::RSquare, "]");
            RSquare rsquare(ts.token()->span());
            ts.advance();

            lhs = std::make_shared<IndexingExpression>(lhs, lsquare, index,
                                                       rsquare);
        } else if (ts.token()->kind() == TokenKind::LParen) {
            LParen lparen(ts.token()->span());
            ts.advance();

            std::vector<std::shared_ptr<Expression>> params;
            while (true) {
                if (ts.token()->kind() == TokenKind::RParen) {
                    RParen rparen(ts.token()->span());
                    lhs = std::make_shared<CallingExpression>(lhs, lparen,
                                                              params, rparen);
                    ts.advance();
                    break;
                }

                params.push_back(parse_assign_expr(ts));

                check(ts);
                if (ts.token()->kind() == TokenKind::RParen) {
                    RParen rparen(ts.token()->span());
                    lhs = std::make_shared<CallingExpression>(lhs, lparen,
                                                              params, rparen);
                    ts.advance();
                    break;
                } else if (ts.token()->kind() == TokenKind::Comma) {
                    ts.advance();
                } else {
                    throw ParseError("expected ) or ,", ts.token()->span());
                }
            }
        } else if (ts.token()->kind() == TokenKind::Dot) {
            AccessExpressionOp op(AccessExpressionOpKind::Dot,
                                  ts.token()->span());
            ts.advance();

            check(ts, TokenKind::Identifier, "identifier");
            auto tk =
                std::static_pointer_cast<ValueToken<std::string>>(ts.token());
            auto name = tk->value();
            AccessExpressionField field(name, ts.token()->span());
            ts.advance();

            lhs = std::make_shared<AccessExpression>(lhs, op, field);
        } else if (ts.token()->kind() == TokenKind::Arrow) {
            AccessExpressionOp op(AccessExpressionOpKind::Arrow,
                                  ts.token()->span());
            ts.advance();

            check(ts, TokenKind::Identifier, "identifier");
            auto tk =
                std::static_pointer_cast<ValueToken<std::string>>(ts.token());
            auto name = tk->value();
            AccessExpressionField field(name, ts.token()->span());
            ts.advance();

            lhs = std::make_shared<AccessExpression>(lhs, op, field);
        } else if (ts.token()->kind() == TokenKind::PlusPlus) {
            PostfixExpressionOp op(PostfixExpressionOpKind::Inc,
                                   ts.token()->span());
            ts.advance();
            lhs = std::make_shared<PostfixExpression>(lhs, op);
        } else if (ts.token()->kind() == TokenKind::MinusMinus) {
            PostfixExpressionOp op(PostfixExpressionOpKind::Dec,
                                   ts.token()->span());
            ts.advance();
            lhs = std::make_shared<PostfixExpression>(lhs, op);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_primary_expr(TokenStream& ts) {
    check(ts);
    if (ts.token()->kind() == TokenKind::Identifier) {
        auto tk = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        auto value = tk->value();
        auto span = tk->span();
        ts.advance();
        return std::make_shared<IdentifierExpression>(value, span);
    } else if (ts.token()->kind() == TokenKind::Integer) {
        auto tk = std::static_pointer_cast<ValueToken<long long>>(ts.token());
        auto value = tk->value();
        auto span = tk->span();
        ts.advance();
        return std::make_shared<IntegerExpression>(value, span);
    } else if (ts.token()->kind() == TokenKind::LParen) {
        LParen lparen(ts.token()->span());
        ts.advance();

        auto expr = parse_expr(ts);

        check(ts, TokenKind::RParen, ")");
        RParen rparen(ts.token()->span());
        ts.advance();

        return std::make_shared<SurroundedExpression>(lparen, expr, rparen);
    } else {
        throw ParseError("expected one of identifier, integer or (",
                         ts.token()->span());
    }
}

// ==================== program parser ====================

Program parse(Context& ctx, std::istream& is, const std::string& name,
              std::vector<ParseError>& es) {
    std::vector<LexError> les;
    auto ts = preprocess(ctx, lex(ctx, is, name, les));
    for (const auto le : les) {
        // TODO: Better way to treant lex error?
        es.emplace_back(le.what(), le.span());
    }

    std::vector<std::shared_ptr<Declaration>> decls;
    while (!ts.eos()) {
        try {
            decls.emplace_back(parse_decl(ts));
        } catch (ParseError e) {
            es.emplace_back(e);
            return decls;
        }
    }
    return decls;
}

}  // namespace tinyc
