#include "parser.h"

#include <iostream>
#include <istream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "../lexer/lexer.h"
#include "../report/report.h"
#include "../report/reportable.h"
#include "decl.h"
#include "node.h"
#include "stmt.h"
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
        throw SimpleError("expected token after this", std::nullopt,
                          ts.token()->span());
    }
}

void check(TokenStream& ts, TokenKind kind, const std::string& msg) {
    check(ts);
    if (ts.token()->kind() == kind) return;
    if (ts.retrest())
        throw SimpleError("expected " + msg + " after this", std::nullopt,
                          ts.token()->span());
    else
        throw SimpleError("expected this to be " + msg, std::nullopt,
                          ts.token()->span());
}

// ==================== type parser ====================

std::shared_ptr<EnumTypeSpecifier> parse_enum(Context& ctx, TokenStream& ts) {
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
                init.emplace(assign, parse_expr(ctx, ts));
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
                throw SimpleError("expected , or } after enumerator",
                                  std::nullopt, ts.token()->span());
            }
        }
    } else if (name.has_value()) {
        return std::make_shared<NamedEnumTypeSpecifier>(enum_kw, name.value());
    } else {
        throw SimpleError("expected either block or identifier after `enum`",
                          std::nullopt, ts.token()->span());
    }
}

std::shared_ptr<UnionTypeSpecifier> parse_union(Context& ctx, TokenStream& ts) {
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

        std::vector<VariablesDeclaration> decls;
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
            auto decl = parse_var_decl(ctx, ts);
            for (const auto& d : decl->decls()) {
                if (d->class_specifier().has_value()) {
                    throw SimpleError(
                        "class specifier in union member is not allowed",
                        std::nullopt, d->span());
                }
            }
            decls.emplace_back(*decl);
        }
    } else if (name.has_value()) {
        return std::make_shared<NamedUnionTypeSpecifier>(union_kw,
                                                         name.value());
    } else {
        throw SimpleError("expected either block or identifier after `union`",
                          std::nullopt, ts.token()->span());
    }
}

std::shared_ptr<StructTypeSpecifier> parse_struct(Context& ctx,
                                                  TokenStream& ts) {
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

        std::vector<VariablesDeclaration> decls;
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
            auto decl = parse_var_decl(ctx, ts);
            for (const auto& d : decl->decls()) {
                if (d->class_specifier().has_value()) {
                    throw SimpleError(
                        "class specifier in struct member is not allowed",
                        std::nullopt, d->span());
                }
            }
            decls.emplace_back(*decl);
        }
    } else if (name.has_value()) {
        return std::make_shared<NamedStructTypeSpecifier>(struct_kw,
                                                          name.value());
    } else {
        throw SimpleError("expected either block or identifier after `struct`",
                          std::nullopt, ts.token()->span());
    }
}

// ==================== declaration parser ====================

std::shared_ptr<Declaration> parse_decl(Context& ctx, TokenStream& ts) {
    auto [storage, concrete] = parse_decl_head(ctx, ts);
    auto state = ts.get_state();
    std::shared_ptr<Type> base = concrete;
    auto body = parse_decl_body(ctx, ts, base);
    if (body.is_variable()) {
        ts.set_state(state);
        std::vector<std::shared_ptr<VariableDeclaration>> decls;
        while (true) {
            auto body = parse_decl_body(ctx, ts, base);
            if (body.is_function()) {
                throw SimpleError("expected variable, but function found",
                                  std::nullopt, body.span());
            }
            auto type = body.type();
            auto name = body.variable();

            if (name.has_value() && !ts.eos() &&
                ts.token()->kind() == TokenKind::Assign) {
                Assign assign(ts.token()->span());
                ts.advance();
                auto init = parse_var_decl_init(ctx, ts);

                decls.emplace_back(std::make_shared<NamedVariableDeclaration>(
                    storage, type, name.value(), std::make_pair(assign, init)));
            } else {
                if (name.has_value())
                    decls.emplace_back(
                        std::make_shared<NamedVariableDeclaration>(
                            storage, type, name.value()));
                else
                    decls.emplace_back(
                        std::make_shared<AnonymousVariableDeclaration>(storage,
                                                                       type));
            }

            if (!ts.eos() && ts.token()->kind() == TokenKind::Comma) {
                ts.advance();
            } else if (!ts.eos() &&
                       ts.token()->kind() == TokenKind::Semicolon) {
                Semicolon semicolon(ts.token()->span());
                ts.advance();
                return std::make_shared<VariablesDeclaration>(decls, semicolon);
            } else {
                ts.retrest();
                throw SimpleError("expected , or ; after this", std::nullopt,
                                  ts.token()->span());
            }
        }
    } else {
        auto ret_type = body.type();
        auto [name, lparen, args, rparen] = body.function();
        check(ts);
        if (ts.token()->kind() == TokenKind::Semicolon) {
            Semicolon semicolon(ts.token()->span());
            ts.advance();
            return std::make_shared<FunctionDeclaration>(
                storage, ret_type, name, lparen, args, rparen, semicolon);
        } else if (ts.token()->kind() == TokenKind::LCurly) {
            auto body = parse_block_stmt(ctx, ts);
            if (!ts.eos() && ts.token()->kind() == TokenKind::Semicolon) {
                Semicolon semicolon(ts.token()->span());
                ts.advance();
                return std::make_shared<FunctionDeclaration>(
                    storage, ret_type, name, lparen, args, rparen, body,
                    semicolon);
            } else {
                return std::make_shared<FunctionDeclaration>(
                    storage, ret_type, name, lparen, args, rparen, body);
            }
        } else {
            ts.retrest();
            throw SimpleError("expected ; or { after this", std::nullopt,
                              ts.token()->span());
        }
    }
}

std::shared_ptr<VariablesDeclaration> parse_var_decl(Context& ctx,
                                                     TokenStream& ts) {
    auto decl = parse_decl(ctx, ts);
    if (decl->kind() == DeclarationKind::Variables) {
        return std::static_pointer_cast<VariablesDeclaration>(decl);
    } else {
        throw SimpleError("expected variables, but got function", std::nullopt,
                          decl->span());
    }
}

std::shared_ptr<FunctionDeclaration> parse_fun_decl(Context& ctx,
                                                    TokenStream& ts) {
    auto decl = parse_decl(ctx, ts);
    if (decl->kind() == DeclarationKind::Function) {
        return std::static_pointer_cast<FunctionDeclaration>(decl);
    } else {
        throw SimpleError("expected function, but got variables", std::nullopt,
                          decl->span());
    }
}

bool collect_storage_class_specifiers(
    Context& ctx, TokenStream& ts,
    std::vector<std::shared_ptr<Token>>& storage_class_specifiers) {
    bool collect_atleast_one = false;
    while (!ts.eos()) {
        auto kind = ts.token()->kind();
        if (kind == TokenKind::Auto || kind == TokenKind::Register ||
            kind == TokenKind::Static || kind == TokenKind::Extern ||
            kind == TokenKind::Typedef) {
            collect_atleast_one = true;
            storage_class_specifiers.push_back(ts.token());
            ts.advance();
        } else {
            break;
        }
    }
    return collect_atleast_one;
}

bool collect_type_quantifiers(
    Context& ctx, TokenStream& ts,
    std::vector<std::shared_ptr<Token>>& type_quantifiers) {
    bool collect_atleast_one = false;
    while (!ts.eos()) {
        auto kind = ts.token()->kind();
        if (kind == TokenKind::Const || kind == TokenKind::Volatile) {
            collect_atleast_one = true;
            type_quantifiers.push_back(ts.token());
            ts.advance();
        } else {
            break;
        }
    }
    return collect_atleast_one;
}

bool collect_decl_head_info(
    Context& ctx, TokenStream& ts,
    std::vector<std::shared_ptr<Token>>& storage_class_specifiers,
    std::vector<std::shared_ptr<Token>>& type_quantifiers) {
    bool collect_atleast_one = false;
    while (true) {
        if (collect_storage_class_specifiers(ctx, ts,
                                             storage_class_specifiers)) {
            collect_atleast_one = true;
            continue;
        }
        if (collect_type_quantifiers(ctx, ts, type_quantifiers)) {
            collect_atleast_one = true;
            continue;
        }
        break;
    }
    return collect_atleast_one;
}

bool collect_builtin_types(Context& ctx, TokenStream& ts,
                           std::vector<std::shared_ptr<Token>>& builtin_types) {
    bool collect_atleast_one = false;
    while (!ts.eos()) {
        auto kind = ts.token()->kind();
        if (kind == TokenKind::Void || kind == TokenKind::Signed ||
            kind == TokenKind::Unsigned || kind == TokenKind::Char ||
            kind == TokenKind::Short || kind == TokenKind::Int ||
            kind == TokenKind::Long || kind == TokenKind::Float ||
            kind == TokenKind::Double) {
            collect_atleast_one = true;
            builtin_types.push_back(ts.token());
            ts.advance();
        } else {
            break;
        }
    }
    return collect_atleast_one;
}

std::optional<StorageClassSpecifier> verify_storage_class_specifiers(
    Context& ctx,
    std::vector<std::shared_ptr<Token>>& storage_class_specifiers) {
    std::optional<std::shared_ptr<Token>> flatten;
    for (const auto& s : storage_class_specifiers) {
        if (!flatten.has_value()) flatten.emplace(s);
        if (flatten.value()->kind() != s->kind()) {
            std::vector<Span> spans;
            for (const auto& s : storage_class_specifiers)
                spans.emplace_back(s->span());
            throw SimpleError("more than one storage class", std::nullopt,
                              concat_spans(spans));
        }
    }
    if (!flatten.has_value()) {
        return std::nullopt;
    } else {
        if (storage_class_specifiers.size() > 1) {
            for (int i = 1; i < storage_class_specifiers.size(); i++) {
                Span span = storage_class_specifiers[i]->span();
                report(ctx, SimpleWarning("multiple storage class",
                                          "remove this", span));
            }
        }
        auto kind = flatten.value()->kind();
        auto span = flatten.value()->span();
        if (kind == TokenKind::Auto) {
            return StorageClassSpecifier(StorageClassSpecifierKind::Auto, span);
        } else if (kind == TokenKind::Register) {
            return StorageClassSpecifier(StorageClassSpecifierKind::Register,
                                         span);
        } else if (kind == TokenKind::Static) {
            return StorageClassSpecifier(StorageClassSpecifierKind::Static,
                                         span);
        } else if (kind == TokenKind::Extern) {
            return StorageClassSpecifier(StorageClassSpecifierKind::Extern,
                                         span);
        } else if (kind == TokenKind::Typedef) {
            return StorageClassSpecifier(StorageClassSpecifierKind::Typedef,
                                         span);
        } else {
            throw SimpleError("unknown storage class", std::nullopt, span);
        }
    }
}

std::pair<std::optional<Const>, std::optional<Volatile>>
verify_type_quantifiers(Context& ctx,
                        std::vector<std::shared_ptr<Token>>& type_quantifiers) {
    std::optional<Const> const_kw;
    std::optional<Volatile> volatile_kw;
    for (const auto& q : type_quantifiers) {
        if (q->kind() == TokenKind::Const) {
            if (const_kw.has_value()) {
                auto e = SimpleWarning("multiple const", "remove this const",
                                       q->span());
                report(ctx, e);
            } else {
                const_kw.emplace(Const(q->span()));
            }
        } else if (q->kind() == TokenKind::Volatile) {
            if (volatile_kw.has_value()) {
                auto e = SimpleWarning("multiple volatile",
                                       "remove this volatile", q->span());
                report(ctx, e);
            } else {
                volatile_kw.emplace(Volatile(q->span()));
            }
        } else {
            throw SimpleError("unknown type quantifier", std::nullopt,
                              q->span());
        }
    }
    return {const_kw, volatile_kw};
}

std::shared_ptr<BuiltinTypeSpecifier> verify_builtin_types(
    Context& ctx, std::vector<std::shared_ptr<Token>>& builtin_types) {
    using k1 = TokenKind;
    using k2 = BuiltinTypeSpecifierKind;

    std::multiset<TokenKind> kinds;
    for (const auto& t : builtin_types) {
        if (t->kind() == TokenKind::Signed) {
            if (kinds.find(t->kind()) != kinds.end()) {
                auto e = SimpleWarning("multiple signed", "remove this signed",
                                       t->span());
                report(ctx, e);
                continue;
            } else if (kinds.find(TokenKind::Unsigned) != kinds.end()) {
                throw SimpleError("combine with signed is not allowed",
                                  std::nullopt, t->span());
            }
        } else if (t->kind() == TokenKind::Unsigned) {
            if (kinds.find(t->kind()) != kinds.end()) {
                auto e = SimpleWarning("multiple unsigned",
                                       "remove this unsigned", t->span());
                report(ctx, e);
                continue;
            } else if (kinds.find(TokenKind::Signed) != kinds.end()) {
                throw SimpleError("combine with unsigned is not allowed",
                                  std::nullopt, t->span());
            }
        }
        kinds.emplace(t->kind());
    }

    std::vector<Span> spans;
    for (const auto& t : builtin_types) spans.emplace_back(t->span());
    auto span = concat_spans(spans);

    std::map<std::multiset<TokenKind>, BuiltinTypeSpecifierKind> map;
    map.insert({{k1::Void}, k2::Void});
    map.insert({{k1::Char}, k2::Char});
    map.insert({{k1::Signed, k1::Char}, k2::Char});
    map.insert({{k1::Unsigned, k1::Char}, k2::UnsignedChar});
    map.insert({{k1::Short}, k2::Short});
    map.insert({{k1::Short, k1::Int}, k2::Short});
    map.insert({{k1::Signed, k1::Short}, k2::Short});
    map.insert({{k1::Signed, k1::Short, k1::Int}, k2::Short});
    map.insert({{k1::Unsigned, k1::Short}, k2::UnsignedShort});
    map.insert({{k1::Unsigned, k1::Short, k1::Int}, k2::UnsignedShort});
    map.insert({{k1::Int}, k2::Int});
    map.insert({{k1::Signed}, k2::Int});
    map.insert({{k1::Signed, k1::Int}, k2::Int});
    map.insert({{k1::Unsigned}, k2::UnsignedInt});
    map.insert({{k1::Unsigned, k1::Int}, k2::UnsignedInt});
    map.insert({{k1::Long}, k2::Long});
    map.insert({{k1::Long, k1::Int}, k2::Long});
    map.insert({{k1::Signed, k1::Long}, k2::Long});
    map.insert({{k1::Signed, k1::Long, k1::Int}, k2::Long});
    map.insert({{k1::Unsigned, k1::Long}, k2::UnsignedLong});
    map.insert({{k1::Unsigned, k1::Long, k1::Int}, k2::UnsignedLong});
    map.insert({{k1::Long, k1::Long}, k2::LongLong});
    map.insert({{k1::Long, k1::Long, k1::Int}, k2::LongLong});
    map.insert({{k1::Signed, k1::Long, k1::Long}, k2::LongLong});
    map.insert({{k1::Signed, k1::Long, k1::Long, k1::Int}, k2::LongLong});
    map.insert({{k1::Unsigned, k1::Long, k1::Long}, k2::UnsignedLongLong});
    map.insert(
        {{k1::Unsigned, k1::Long, k1::Long, k1::Int}, k2::UnsignedLongLong});
    map.insert({{k1::Float}, k2::Float});
    map.insert({{k1::Double}, k2::Double});
    map.insert({{k1::Long, k1::Double}, k2::LongDouble});

    try {
        auto kind = map.at(kinds);
        return std::make_shared<BuiltinTypeSpecifier>(kind, span);
    } catch (std::out_of_range e) {
        throw SimpleError("unknown type", std::nullopt, span);
    }
}

std::pair<std::optional<StorageClassSpecifier>, std::shared_ptr<ConcreteType>>
parse_decl_head(Context& ctx, TokenStream& ts) {
    std::vector<std::shared_ptr<Token>> storage_class_specifiers;
    std::vector<std::shared_ptr<Token>> type_quantifiers;
    collect_decl_head_info(ctx, ts, storage_class_specifiers, type_quantifiers);
    check(ts);
    if (ts.token()->kind() == TokenKind::Identifier) {
        auto id = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        auto spec =
            std::make_shared<TypedefNameTypeSpecifier>(id->value(), id->span());
        ts.advance();

        collect_decl_head_info(ctx, ts, storage_class_specifiers,
                               type_quantifiers);
        auto storage =
            verify_storage_class_specifiers(ctx, storage_class_specifiers);
        auto [const_kw, volatile_kw] =
            verify_type_quantifiers(ctx, type_quantifiers);

        return {storage,
                std::make_shared<ConcreteType>(spec, const_kw, volatile_kw)};
    } else if (ts.token()->kind() == TokenKind::Enum) {
        auto spec = parse_enum(ctx, ts);

        collect_decl_head_info(ctx, ts, storage_class_specifiers,
                               type_quantifiers);
        auto storage =
            verify_storage_class_specifiers(ctx, storage_class_specifiers);
        auto [const_kw, volatile_kw] =
            verify_type_quantifiers(ctx, type_quantifiers);

        return {storage,
                std::make_shared<ConcreteType>(spec, const_kw, volatile_kw)};
    } else if (ts.token()->kind() == TokenKind::Union) {
        auto spec = parse_union(ctx, ts);

        collect_decl_head_info(ctx, ts, storage_class_specifiers,
                               type_quantifiers);
        auto storage =
            verify_storage_class_specifiers(ctx, storage_class_specifiers);
        auto [const_kw, volatile_kw] =
            verify_type_quantifiers(ctx, type_quantifiers);

        return {storage,
                std::make_shared<ConcreteType>(spec, const_kw, volatile_kw)};
    } else if (ts.token()->kind() == TokenKind::Struct) {
        auto spec = parse_struct(ctx, ts);

        collect_decl_head_info(ctx, ts, storage_class_specifiers,
                               type_quantifiers);
        auto storage =
            verify_storage_class_specifiers(ctx, storage_class_specifiers);
        auto [const_kw, volatile_kw] =
            verify_type_quantifiers(ctx, type_quantifiers);

        return {storage,
                std::make_shared<ConcreteType>(spec, const_kw, volatile_kw)};
    } else {
        std::vector<std::shared_ptr<Token>> builtin_types;
        while (true) {
            bool collect_atleast_one = false;
            collect_atleast_one |= collect_decl_head_info(
                ctx, ts, storage_class_specifiers, type_quantifiers);
            collect_atleast_one |=
                collect_builtin_types(ctx, ts, builtin_types);
            if (!collect_atleast_one) {
                break;
            }
        }
        auto spec = verify_builtin_types(ctx, builtin_types);
        auto storage =
            verify_storage_class_specifiers(ctx, storage_class_specifiers);
        auto [const_kw, volatile_kw] =
            verify_type_quantifiers(ctx, type_quantifiers);

        return {storage,
                std::make_shared<ConcreteType>(spec, const_kw, volatile_kw)};
    }
}

VariableOrFunctionDeclBody parse_decl_body(Context& ctx, TokenStream& ts,
                                           const std::shared_ptr<Type>& base) {
    std::shared_ptr<Type> type = base;

    while (true) {
        if (!ts.eos() && ts.token()->kind() == TokenKind::Star) {
            Star star(ts.token()->span());
            ts.advance();

            std::vector<std::shared_ptr<Token>> quantifiers;
            collect_type_quantifiers(ctx, ts, quantifiers);
            auto [const_kw, volatile_kw] =
                verify_type_quantifiers(ctx, quantifiers);

            type = std::make_shared<PointerType>(star, const_kw, volatile_kw,
                                                 type);
        } else {
            break;
        }
    }

    if (ts.eos()) {
        return VariableOrFunctionDeclBody(type, std::nullopt);
    } else if (ts.token()->kind() == TokenKind::Identifier) {
        auto id = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        ts.advance();

        if (ts.eos()) {
            VariableDeclarationName name(id->value(), id->span());
            return VariableOrFunctionDeclBody(type, name);
        } else if (ts.token()->kind() == TokenKind::LParen) {
            auto [lparen, params, rparen] = parse_fun_params(ctx, ts);
            FunctionDeclarationName name(id->value(), id->span());
            return VariableOrFunctionDeclBody(type,
                                              {name, lparen, params, rparen});
        } else if (ts.token()->kind() == TokenKind::LSquare) {
            type = parse_arrays(ctx, ts, type);
            VariableDeclarationName name(id->value(), id->span());
            return VariableOrFunctionDeclBody(type, name);
        } else {
            VariableDeclarationName name(id->value(), id->span());
            return VariableOrFunctionDeclBody(type, name);
        }
    } else if (ts.token()->kind() == TokenKind::LParen) {
        ts.advance();

        auto inner_state = ts.get_state();
        int waiting_paren = 1;
        while (true) {
            if (ts.eos()) {
                throw SimpleError("unclosing parenthesis in type", std::nullopt,
                                  ts.last()->span());
            }
            if (ts.token()->kind() == TokenKind::RParen) {
                waiting_paren--;
                ts.advance();
                if (waiting_paren == 0) {
                    break;
                }
            } else if (ts.token()->kind() == TokenKind::LParen) {
                waiting_paren++;
                ts.advance();
            } else {
                ts.advance();
            }
        }

        if (ts.eos()) {
            auto state = ts.get_state();

            ts.set_state(inner_state);
            auto ret = parse_decl_body(ctx, ts, type);
            ts.set_state(state);
            return ret;
        } else if (ts.token()->kind() == TokenKind::LParen) {
            auto [lparen, params, rparen] = parse_fun_params(ctx, ts);
            type = std::make_shared<FunctionType>(type, lparen, params, rparen);
            auto state = ts.get_state();

            ts.set_state(inner_state);
            auto ret = parse_decl_body(ctx, ts, type);
            ts.set_state(state);
            return ret;
        } else if (ts.token()->kind() == TokenKind::LSquare) {
            type = parse_arrays(ctx, ts, type);
            auto state = ts.get_state();

            ts.set_state(inner_state);
            auto ret = parse_decl_body(ctx, ts, type);
            ts.set_state(state);
            return ret;
        } else {
            auto state = ts.get_state();

            ts.set_state(inner_state);
            auto ret = parse_decl_body(ctx, ts, type);
            ts.set_state(state);
            return ret;
        }
    } else {
        return VariableOrFunctionDeclBody(type, std::nullopt);
    }
}

std::shared_ptr<Type> parse_arrays(Context& ctx, TokenStream& ts,
                                   const std::shared_ptr<Type>& base) {
    auto type = base;
    while (!ts.eos()) {
        if (ts.token()->kind() != TokenKind::LSquare) {
            break;
        }
        LSquare lsquare(ts.token()->span());
        ts.advance();

        std::optional<std::shared_ptr<Expression>> size;
        check(ts);
        if (ts.token()->kind() != TokenKind::RSquare) {
            size = parse_cond_expr(ctx, ts);
        }

        check(ts, TokenKind::RSquare, "]");
        RSquare rsquare(ts.token()->span());
        ts.advance();

        if (size.has_value())
            type = std::make_shared<ArrayType>(type, lsquare, size.value(),
                                               rsquare);
        else
            type = std::make_shared<ArrayType>(type, lsquare, rsquare);
    }
    return type;
}

std::tuple<LParen, std::vector<FunctionParam>, RParen> parse_fun_params(
    Context& ctx, TokenStream& ts) {
    check(ts, TokenKind::LParen, "(");
    LParen lparen(ts.token()->span());
    ts.advance();

    std::vector<FunctionParam> params;
    while (true) {
        if (!ts.eos() && ts.token()->kind() == TokenKind::RParen) {
            RParen rparen(ts.token()->span());
            ts.advance();
            return {lparen, params, rparen};
        }

        auto [storage, concrete] = parse_decl_head(ctx, ts);
        std::shared_ptr<Type> base = concrete;
        auto body = parse_decl_body(ctx, ts, base);

        if (body.is_function()) {
            throw SimpleError("expected variable, but got function",
                              std::nullopt, body.span());
        }

        auto param_type = body.type();
        if (body.variable().has_value()) {
            FunctionParamName param_name(body.variable()->name(),
                                         body.variable()->span());
            params.emplace_back(param_type, param_name);
        } else {
            params.emplace_back(param_type);
        }

        if (!ts.eos() && ts.token()->kind() == TokenKind::RParen) {
            continue;
        } else if (!ts.eos() && ts.token()->kind() == TokenKind::Comma) {
            ts.advance();
        } else {
            ts.retrest();
            throw SimpleError("expected ) or , after this", std::nullopt,
                              ts.token()->span());
        }
    }
}

std::shared_ptr<VariableDeclarationInit> parse_var_decl_init(Context& ctx,
                                                             TokenStream& ts) {
    check(ts);
    if (ts.token()->kind() == TokenKind::LCurly) {
        LCurly lcurly(ts.token()->span());
        ts.advance();

        std::vector<std::shared_ptr<VariableDeclarationInit>> inits;
        while (true) {
            check(ts);
            if (ts.token()->kind() == TokenKind::RCurly) {
                RCurly rcurly(ts.token()->span());
                ts.advance();

                return std::make_shared<VariableDeclarationInitList>(
                    lcurly, inits, rcurly);
            }

            inits.emplace_back(parse_var_decl_init(ctx, ts));

            check(ts);
            if (ts.token()->kind() == TokenKind::Comma) {
                ts.advance();
            }
        }
    } else {
        auto expr = parse_assign_expr(ctx, ts);
        return std::make_shared<VariableDeclarationInitExpr>(expr);
    }
}

// ==================== statement parser ====================

std::shared_ptr<Statement> parse_stmt(Context& ctx, TokenStream& ts) {
    check(ts);
    if (ts.token()->kind() == TokenKind::Identifier) {
        ts.advance();
        if (!ts.eos() && ts.token()->kind() == TokenKind::Colon) {
            return parse_labeled_stmt(ctx, ts);
        } else {
            ts.retrest();
            return parse_expr_stmt(ctx, ts);
        }
    } else if (ts.token()->kind() == TokenKind::Case) {
        return parse_case_stmt(ctx, ts);
    } else if (ts.token()->kind() == TokenKind::Default) {
        return parse_default_stmt(ctx, ts);
    } else if (ts.token()->kind() == TokenKind::LCurly) {
        return parse_block_stmt(ctx, ts);
    } else if (ts.token()->kind() == TokenKind::If) {
        return parse_if_stmt(ctx, ts);
    } else if (ts.token()->kind() == TokenKind::Switch) {
        return parse_switch_stmt(ctx, ts);
    } else if (ts.token()->kind() == TokenKind::While) {
        return parse_while_stmt(ctx, ts);
    } else if (ts.token()->kind() == TokenKind::Do) {
        return parse_do_while_stmt(ctx, ts);
    } else if (ts.token()->kind() == TokenKind::For) {
        return parse_for_stmt(ctx, ts);
    } else if (ts.token()->kind() == TokenKind::Goto) {
        return parse_goto_stmt(ctx, ts);
    } else if (ts.token()->kind() == TokenKind::Break) {
        return parse_break_stmt(ctx, ts);
    } else if (ts.token()->kind() == TokenKind::Return) {
        return parse_return_stmt(ctx, ts);
    } else {
        return parse_expr_stmt(ctx, ts);
    }
}

std::shared_ptr<LabeledStatement> parse_labeled_stmt(Context& ctx,
                                                     TokenStream& ts) {
    check(ts, TokenKind::Identifier, "identifier");
    auto tk = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
    LabeledStatementLabel label(tk->value(), tk->span());
    ts.advance();

    check(ts, TokenKind::Colon, ":");
    Colon colon(ts.token()->span());
    ts.advance();

    auto stmt = parse_stmt(ctx, ts);

    return std::make_shared<LabeledStatement>(label, colon, stmt);
}

std::shared_ptr<CaseStatement> parse_case_stmt(Context& ctx, TokenStream& ts) {
    check(ts, TokenKind::Case, "case");
    Case case_kw(ts.token()->span());
    ts.advance();

    auto expr = parse_expr(ctx, ts);

    check(ts, TokenKind::Colon, ":");
    Colon colon(ts.token()->span());
    ts.advance();

    auto stmt = parse_stmt(ctx, ts);

    return std::make_shared<CaseStatement>(case_kw, expr, colon, stmt);
}

std::shared_ptr<DefaultStatement> parse_default_stmt(Context& ctx,
                                                     TokenStream& ts) {
    check(ts, TokenKind::Default, "default");
    Default default_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Colon, ":");
    Colon colon(ts.token()->span());
    ts.advance();

    auto stmt = parse_stmt(ctx, ts);

    return std::make_shared<DefaultStatement>(default_kw, colon, stmt);
}

std::shared_ptr<BlockStatement> parse_block_stmt(Context& ctx,
                                                 TokenStream& ts) {
    check(ts, TokenKind::LCurly, "{");
    LCurly lcurly(ts.token()->span());
    ts.advance();

    std::vector<std::shared_ptr<BlockStatementItem>> items;
    while (true) {
        if (ts.eos()) {
            std::vector<Span> spans;
            spans.emplace_back(lcurly.span());
            for (const auto& item : items) spans.emplace_back(item->span());
            throw SimpleError("unclosing block statement", std::nullopt,
                              concat_spans(spans));
        }
        if (ts.token()->kind() == TokenKind::RCurly) {
            RCurly rcurly(ts.token()->span());
            ts.advance();
            return std::make_shared<BlockStatement>(lcurly, items, rcurly);
        }

        auto state = ts.get_state();
        try {
            auto stmt = parse_stmt(ctx, ts);
            items.push_back(
                std::make_shared<BlockStatementStatementItem>(stmt));
        } catch (SimpleError e) {
            ts.set_state(state);
            auto decl = parse_decl(ctx, ts);
            items.push_back(
                std::make_shared<BlockStatementDeclarationItem>(decl));
        }
    }
}

std::shared_ptr<IfStatement> parse_if_stmt(Context& ctx, TokenStream& ts) {
    check(ts, TokenKind::If, "if");
    If if_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    LParen lparen(ts.token()->span());
    ts.advance();

    auto cond = parse_expr(ctx, ts);

    check(ts, TokenKind::RParen, ")");
    RParen rparen(ts.token()->span());
    ts.advance();

    auto then = parse_stmt(ctx, ts);

    IfStatementIfBody if_body(if_kw, lparen, cond, rparen, then);

    if (ts.eos() || ts.token()->kind() != TokenKind::Else) {
        return std::make_shared<IfStatement>(if_body);
    }

    Else else_kw(ts.token()->span());
    ts.advance();

    auto otherwise = parse_stmt(ctx, ts);

    IfStatementElseBody else_body = IfStatementElseBody(else_kw, otherwise);

    return std::make_shared<IfStatement>(if_body, else_body);
}

std::shared_ptr<SwitchStatement> parse_switch_stmt(Context& ctx,
                                                   TokenStream& ts) {
    check(ts, TokenKind::Switch, "switch");
    Switch switch_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    LParen lparen(ts.token()->span());
    ts.advance();

    auto comp = parse_expr(ctx, ts);

    check(ts, TokenKind::RParen, ")");
    RParen rparen(ts.token()->span());
    ts.advance();

    auto body = parse_stmt(ctx, ts);

    return std::make_shared<SwitchStatement>(switch_kw, lparen, comp, rparen,
                                             body);
}

std::shared_ptr<WhileStatement> parse_while_stmt(Context& ctx,
                                                 TokenStream& ts) {
    check(ts, TokenKind::While, "while");
    While switch_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    LParen lparen(ts.token()->span());
    ts.advance();

    auto cond = parse_expr(ctx, ts);

    check(ts, TokenKind::RParen, ")");
    RParen rparen(ts.token()->span());
    ts.advance();

    auto body = parse_stmt(ctx, ts);

    return std::make_shared<WhileStatement>(switch_kw, lparen, cond, rparen,
                                            body);
}

std::shared_ptr<DoWhileStatement> parse_do_while_stmt(Context& ctx,
                                                      TokenStream& ts) {
    check(ts, TokenKind::Do, "do");
    Do do_kw(ts.token()->span());
    ts.advance();

    auto body = parse_stmt(ctx, ts);

    check(ts, TokenKind::While, "while");
    While while_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    LParen lparen(ts.token()->span());
    ts.advance();

    auto cond = parse_expr(ctx, ts);

    check(ts, TokenKind::RParen, ")");
    RParen rparen(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Semicolon, ";");
    Semicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<DoWhileStatement>(do_kw, body, while_kw, lparen,
                                              cond, rparen, semicolon);
}

std::shared_ptr<ForStatement> parse_for_stmt(Context& ctx, TokenStream& ts) {
    check(ts, TokenKind::For, "for");
    For for_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::LParen, "(");
    LParen lparen(ts.token()->span());
    ts.advance();

    std::optional<std::shared_ptr<ForStatementInit>> init = std::nullopt;
    check(ts);
    if (ts.token()->kind() != TokenKind::Semicolon) {
        auto state = ts.get_state();
        try {
            auto expr = parse_expr(ctx, ts);

            check(ts, TokenKind::Semicolon, ";");
            Semicolon semicolon(ts.token()->span());
            ts.advance();

            init.emplace(
                std::make_shared<ForStatementInitExpr>(expr, semicolon));
        } catch (SimpleError e) {
            ts.set_state(state);
            auto decl = parse_var_decl(ctx, ts);
            init.emplace(std::make_shared<ForStatementInitDecl>(decl));
        }
    } else {
        Semicolon semicolon(ts.token()->span());
        ts.advance();

        init.emplace(std::make_shared<ForStatementInitExpr>(semicolon));
    }

    std::optional<ForStatementCond> cond = std::nullopt;
    check(ts);
    if (ts.token()->kind() != TokenKind::Semicolon) {
        auto expr = parse_expr(ctx, ts);

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
        update = parse_expr(ctx, ts);
    }
    check(ts, TokenKind::RParen, ")");
    RParen rparen(ts.token()->span());
    ts.advance();

    auto body = parse_stmt(ctx, ts);

    if (update.has_value()) {
        return std::make_shared<ForStatement>(for_kw, lparen, init.value(),
                                              cond.value(), update.value(),
                                              rparen, body);
    } else {
        return std::make_shared<ForStatement>(for_kw, lparen, init.value(),
                                              cond.value(), rparen, body);
    }
}

std::shared_ptr<GotoStatement> parse_goto_stmt(Context& ctx, TokenStream& ts) {
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

std::shared_ptr<ContinueStatement> parse_continue_stmt(Context& ctx,
                                                       TokenStream& ts) {
    check(ts, TokenKind::Continue, "continue");
    Continue continue_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Semicolon, ";");
    Semicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<ContinueStatement>(continue_kw, semicolon);
}

std::shared_ptr<BreakStatement> parse_break_stmt(Context& ctx,
                                                 TokenStream& ts) {
    check(ts, TokenKind::Break, "break");
    Break break_kw(ts.token()->span());
    ts.advance();

    check(ts, TokenKind::Semicolon, ";");
    Semicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<BreakStatement>(break_kw, semicolon);
}

std::shared_ptr<ReturnStatement> parse_return_stmt(Context& ctx,
                                                   TokenStream& ts) {
    check(ts, TokenKind::Return, "return");
    Return return_kw(ts.token()->span());
    ts.advance();

    auto ret = parse_expr(ctx, ts);

    check(ts, TokenKind::Semicolon, ";");
    Semicolon semicolon(ts.token()->span());
    ts.advance();

    return std::make_shared<ReturnStatement>(return_kw, ret, semicolon);
}

std::shared_ptr<ExpressionStatement> parse_expr_stmt(Context& ctx,
                                                     TokenStream& ts) {
    check(ts);
    if (ts.token()->kind() == TokenKind::Semicolon) {
        Semicolon semicolon(ts.token()->span());
        ts.advance();
        return std::make_shared<ExpressionStatement>(semicolon);
    } else {
        auto expr = parse_expr(ctx, ts);

        check(ts, TokenKind::Semicolon, ";");
        Semicolon semicolon(ts.token()->span());
        ts.advance();

        return std::make_shared<ExpressionStatement>(expr, semicolon);
    }
}

// ==================== expression parser ====================

std::shared_ptr<Expression> parse_expr(Context& ctx, TokenStream& ts) {
    auto lhs = parse_assign_expr(ctx, ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Comma) {
            InfixExpressionOp op(InfixExpressionOpKind::Comma,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_assign_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_assign_expr(Context& ctx, TokenStream& ts) {
    auto state = ts.get_state();

    std::shared_ptr<Expression> lhs;
    InfixExpressionOpKind kind;
    try {
        lhs = parse_unary_expr(ctx, ts);
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
                throw SimpleError("expected assign operator", std::nullopt,
                                  ts.token()->span());
        }
    } catch (SimpleError e) {
        ts.set_state(state);
        return parse_cond_expr(ctx, ts);
    }

    InfixExpressionOp op(kind, ts.token()->span());
    ts.advance();

    auto rhs = parse_assign_expr(ctx, ts);
    return std::make_shared<InfixExpression>(lhs, op, rhs);
}

std::shared_ptr<Expression> parse_cond_expr(Context& ctx, TokenStream& ts) {
    auto cond = parse_logical_or_expr(ctx, ts);

    if (ts.eos() || ts.token()->kind() != TokenKind::Question) {
        return cond;
    }
    auto exclamation = Exclamation(ts.token()->span());
    ts.advance();

    auto then = parse_expr(ctx, ts);

    check(ts, TokenKind::Colon, ":");
    auto colon = Colon(ts.token()->span());
    ts.advance();

    auto otherwise = parse_cond_expr(ctx, ts);
    return std::make_shared<ConditionalExpression>(cond, exclamation, then,
                                                   colon, otherwise);
}

std::shared_ptr<Expression> parse_logical_or_expr(Context& ctx,
                                                  TokenStream& ts) {
    auto lhs = parse_logical_and_expr(ctx, ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Or) {
            InfixExpressionOp op(InfixExpressionOpKind::Or, ts.token()->span());
            ts.advance();
            auto rhs = parse_logical_and_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_logical_and_expr(Context& ctx,
                                                   TokenStream& ts) {
    auto lhs = parse_or_expr(ctx, ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::And) {
            InfixExpressionOp op(InfixExpressionOpKind::And,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_or_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_or_expr(Context& ctx, TokenStream& ts) {
    auto lhs = parse_xor_expr(ctx, ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Vertical) {
            InfixExpressionOp op(InfixExpressionOpKind::BitOr,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_xor_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_xor_expr(Context& ctx, TokenStream& ts) {
    auto lhs = parse_and_expr(ctx, ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Hat) {
            InfixExpressionOp op(InfixExpressionOpKind::BitXor,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_and_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_and_expr(Context& ctx, TokenStream& ts) {
    auto lhs = parse_equality_expr(ctx, ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Ampersand) {
            InfixExpressionOp op(InfixExpressionOpKind::BitAnd,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_equality_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_equality_expr(Context& ctx, TokenStream& ts) {
    auto lhs = parse_relative_expr(ctx, ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::EQ) {
            InfixExpressionOp op(InfixExpressionOpKind::EQ, ts.token()->span());
            ts.advance();
            auto rhs = parse_relative_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::NE) {
            InfixExpressionOp op(InfixExpressionOpKind::NE, ts.token()->span());
            ts.advance();
            auto rhs = parse_relative_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_relative_expr(Context& ctx, TokenStream& ts) {
    auto lhs = parse_shift_expr(ctx, ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::LT) {
            InfixExpressionOp op(InfixExpressionOpKind::LT, ts.token()->span());
            ts.advance();
            auto rhs = parse_shift_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::GT) {
            InfixExpressionOp op(InfixExpressionOpKind::GT, ts.token()->span());
            ts.advance();
            auto rhs = parse_shift_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::LE) {
            InfixExpressionOp op(InfixExpressionOpKind::LE, ts.token()->span());
            ts.advance();
            auto rhs = parse_shift_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::GE) {
            InfixExpressionOp op(InfixExpressionOpKind::GE, ts.token()->span());
            ts.advance();
            auto rhs = parse_shift_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_shift_expr(Context& ctx, TokenStream& ts) {
    auto lhs = parse_additive_expr(ctx, ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::LShift) {
            InfixExpressionOp op(InfixExpressionOpKind::LShift,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_additive_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::RShift) {
            InfixExpressionOp op(InfixExpressionOpKind::RShift,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_additive_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_additive_expr(Context& ctx, TokenStream& ts) {
    auto lhs = parse_multiplicative_expr(ctx, ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Plus) {
            InfixExpressionOp op(InfixExpressionOpKind::Add,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_multiplicative_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::Minus) {
            InfixExpressionOp op(InfixExpressionOpKind::Sub,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_multiplicative_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_multiplicative_expr(Context& ctx,
                                                      TokenStream& ts) {
    auto lhs = parse_cast_expr(ctx, ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Star) {
            InfixExpressionOp op(InfixExpressionOpKind::Mul,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_cast_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::Slash) {
            InfixExpressionOp op(InfixExpressionOpKind::Div,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_cast_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else if (ts.token()->kind() == TokenKind::Percent) {
            InfixExpressionOp op(InfixExpressionOpKind::Mod,
                                 ts.token()->span());
            ts.advance();
            auto rhs = parse_cast_expr(ctx, ts);
            lhs = std::make_shared<InfixExpression>(lhs, op, rhs);
        } else {
            break;
        }
    }
    return lhs;
}

std::shared_ptr<Expression> parse_cast_expr(Context& ctx, TokenStream& ts) {
    auto state = ts.get_state();
    std::optional<std::shared_ptr<CastExpression>> cast;
    std::optional<StorageClassSpecifier> storage;
    try {
        check(ts);
        if (ts.token()->kind() != TokenKind::LParen) {
            return parse_unary_expr(ctx, ts);
        }
        LParen lparen(ts.token()->span());
        ts.advance();

        auto [storage_, concrete] = parse_decl_head(ctx, ts);
        std::shared_ptr<Type> base = concrete;
        auto body = parse_decl_body(ctx, ts, base);
        if (body.is_function()) {
            throw SimpleError("expected variable, but got function",
                              std::nullopt, body.span());
        }
        auto type = body.type();

        check(ts, TokenKind::RParen, ")");
        RParen rparen(ts.token()->span());
        ts.advance();

        auto expr = parse_unary_expr(ctx, ts);

        cast = std::make_shared<CastExpression>(lparen, type, rparen, expr);
        if (storage_.has_value()) storage.emplace(storage_.value());
    } catch (SimpleError e) {
        ts.set_state(state);
        return parse_unary_expr(ctx, ts);
    }

    if (storage.has_value()) {
        throw SimpleError("class specifier in cast is not allowed",
                          std::nullopt, storage->span());
    } else {
        return cast.value();
    }
}

std::shared_ptr<Expression> parse_unary_expr(Context& ctx, TokenStream& ts) {
    check(ts);
    if (ts.token()->kind() == TokenKind::PlusPlus) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Inc, ts.token()->span());
        ts.advance();
        auto expr = parse_unary_expr(ctx, ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::MinusMinus) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Dec, ts.token()->span());
        ts.advance();
        auto expr = parse_unary_expr(ctx, ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Ampersand) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Ref, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ctx, ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Star) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Deref, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ctx, ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Plus) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Plus, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ctx, ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Minus) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Minus, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ctx, ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Tilde) {
        UnaryExpressionOp op(UnaryExpressionOpKind::BitInv, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ctx, ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Not) {
        UnaryExpressionOp op(UnaryExpressionOpKind::Neg, ts.token()->span());
        ts.advance();
        auto expr = parse_cast_expr(ctx, ts);
        return std::make_shared<UnaryExpression>(op, expr);
    } else if (ts.token()->kind() == TokenKind::Sizeof) {
        Sizeof sizeof_kw(ts.token()->span());
        ts.advance();
        auto state = ts.get_state();
        try {
            auto expr = parse_unary_expr(ctx, ts);
            return std::make_shared<SizeofExprExpression>(sizeof_kw, expr);
        } catch (SimpleError e) {
            ts.set_state(state);

            check(ts, TokenKind::LParen, "(");
            LParen lparen(ts.token()->span());
            ts.advance();

            auto [storage, concrete] = parse_decl_head(ctx, ts);
            std::shared_ptr<Type> base = concrete;
            auto body = parse_decl_body(ctx, ts, base);
            if (storage.has_value()) {
                throw SimpleError(
                    "class specifier in function param is not allowed",
                    std::nullopt, storage->span());
            }
            if (body.is_function()) {
                throw SimpleError("expected variable, but got function",
                                  std::nullopt, body.span());
            }
            auto type = body.type();

            check(ts, TokenKind::RParen, ")");
            RParen rparen(ts.token()->span());
            ts.advance();

            return std::make_shared<SizeofTypeExpression>(sizeof_kw, lparen,
                                                          type, rparen);
        }
    } else {
        return parse_postfix_expr(ctx, ts);
    }
}

std::shared_ptr<Expression> parse_postfix_expr(Context& ctx, TokenStream& ts) {
    auto lhs = parse_primary_expr(ctx, ts);
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::LSquare) {
            LSquare lsquare(ts.token()->span());
            ts.advance();

            auto index = parse_expr(ctx, ts);

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
                check(ts);
                if (ts.token()->kind() == TokenKind::RParen) {
                    RParen rparen(ts.token()->span());
                    ts.advance();
                    lhs = std::make_shared<CallingExpression>(lhs, lparen,
                                                              params, rparen);
                    break;
                }

                params.push_back(parse_assign_expr(ctx, ts));

                check(ts);
                if (ts.token()->kind() == TokenKind::RParen) {
                    RParen rparen(ts.token()->span());
                    ts.advance();
                    lhs = std::make_shared<CallingExpression>(lhs, lparen,
                                                              params, rparen);
                    break;
                } else if (ts.token()->kind() == TokenKind::Comma) {
                    ts.advance();
                } else {
                    throw SimpleError("expected ) or ,", std::nullopt,
                                      ts.token()->span());
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

std::shared_ptr<Expression> parse_primary_expr(Context& ctx, TokenStream& ts) {
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
    } else if (ts.token()->kind() == TokenKind::String) {
        auto tk = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        auto value = tk->value();
        auto span = tk->span();
        ts.advance();
        return std::make_shared<StringExpression>(value, span);
    } else if (ts.token()->kind() == TokenKind::Character) {
        auto tk = std::static_pointer_cast<ValueToken<char>>(ts.token());
        auto value = tk->value();
        auto span = tk->span();
        ts.advance();
        return std::make_shared<CharacterExpression>(value, span);
    } else if (ts.token()->kind() == TokenKind::LParen) {
        LParen lparen(ts.token()->span());
        ts.advance();

        auto expr = parse_expr(ctx, ts);

        check(ts, TokenKind::RParen, ")");
        RParen rparen(ts.token()->span());
        ts.advance();

        return std::make_shared<SurroundedExpression>(lparen, expr, rparen);
    } else {
        throw SimpleError("expected one of identifier, integer or (",
                          std::nullopt, ts.token()->span());
    }
}

// ==================== program parser ====================

std::optional<Program> parse(Context& ctx, std::istream& is,
                             const std::string& name) {
    auto ts = lex(ctx, is, name);
    if (!ts.has_value()) {
        return std::nullopt;
    }
    std::vector<std::shared_ptr<Declaration>> decls;
    while (!ts->eos()) {
        try {
            decls.emplace_back(parse_decl(ctx, ts.value()));
        } catch (SimpleError e) {
            report(ctx, e);
            return std::nullopt;
        }
    }
    return decls;
}

}  // namespace tinyc
