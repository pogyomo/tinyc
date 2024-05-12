#include "tycheck.h"

#include <memory>
#include <optional>
#include <stdexcept>

#include "../parser/type/specifier/builtin.h"
#include "../parser/type/specifier/struct.h"
#include "../parser/type/specifier/typedef.h"
#include "../parser/type/specifier/union.h"
#include "../report/report.h"

namespace tinyc {

std::optional<bool> is_scalar_type(Context& ctx,
                                   const std::shared_ptr<Type>& type) {
    if (type->kind() == TypeKind::Array || type->kind() == TypeKind::Function) {
        return false;
    } else if (type->kind() == TypeKind::Pointer) {
        return true;
    } else {
        auto concrete = std::static_pointer_cast<ConcreteType>(type);
        if (concrete->specifier()->kind() != TypeSpecifierKind::Struct &&
            concrete->specifier()->kind() != TypeSpecifierKind::Union) {
            return false;
        } else if (concrete->specifier()->kind() ==
                   TypeSpecifierKind::TypedefName) {
            auto name = std::static_pointer_cast<TypedefNameTypeSpecifier>(
                concrete->specifier());
            try {
                return is_scalar_type(ctx,
                                      ctx.symbol_table().get(name->name()));
            } catch (std::out_of_range e) {
                report(ctx, SimpleError("no such type exist", std::nullopt,
                                        name->span()));
                return std::nullopt;
            }
        } else {
            return true;
        }
    }
}

bool is_integer_ty(const std::shared_ptr<Type>& type) {
    if (type->kind() != TypeKind::Concrete) {
        return false;
    } else {
        auto concrete = std::static_pointer_cast<ConcreteType>(type);
        if (concrete->specifier()->kind() != TypeSpecifierKind::Builtin) {
            return false;
        }
        auto builtin = std::static_pointer_cast<BuiltinTypeSpecifier>(
            concrete->specifier());
        if (builtin->builtin_kind() == BuiltinTypeSpecifierKind::Void ||
            builtin->builtin_kind() == BuiltinTypeSpecifierKind::Float ||
            builtin->builtin_kind() == BuiltinTypeSpecifierKind::Double ||
            builtin->builtin_kind() == BuiltinTypeSpecifierKind::LongDouble) {
            return false;
        }
    }
    return true;
}

std::optional<std::shared_ptr<FunctionType>> is_callable(
    Context& ctx, const std::shared_ptr<Type>& type) {
    if (type->kind() == TypeKind::Function) {
        return std::static_pointer_cast<FunctionType>(type);
    } else if (type->kind() == TypeKind::Pointer) {
        auto ptr = std::static_pointer_cast<PointerType>(type);
        if (ptr->of()->kind() == TypeKind::Function) {
            return std::static_pointer_cast<FunctionType>(ptr->of());
        } else {
            return std::nullopt;
        }
    } else {
        return std::nullopt;
    }
}

std::optional<std::shared_ptr<Type>> tyassert(
    Context& ctx, const std::shared_ptr<Expression>& expr) {
    if (expr->kind() == ExpressionKind::Unary) {
        auto unary = std::static_pointer_cast<UnaryExpression>(expr);
        auto expr_ty = tyassert(ctx, unary->expr());
        if (!expr_ty) {
            return std::nullopt;
        }
        if (unary->op().kind() == UnaryExpressionOpKind::Ref) {
            return std::make_shared<PointerType>(Star(expr->span()),
                                                 std::nullopt, std::nullopt,
                                                 expr_ty.value());
        } else if (unary->op().kind() == UnaryExpressionOpKind::Deref) {
            if (expr_ty.value()->kind() == TypeKind::Pointer) {
                auto ptr =
                    std::static_pointer_cast<PointerType>(expr_ty.value());
                return ptr->of();
            } else {
                report(ctx, SimpleError("cannot dereference non-pointer",
                                        std::nullopt, unary->span()));
                return std::nullopt;
            }
        } else if (unary->op().kind() == UnaryExpressionOpKind::Plus) {
            if (is_integer_ty(expr_ty.value())) {
                return expr_ty.value();
            } else {
                report(ctx, SimpleError("cannot apply to non-integer",
                                        std::nullopt, unary->span()));
                return std::nullopt;
            }
        } else if (unary->op().kind() == UnaryExpressionOpKind::Minus) {
            // TODO: Invert sign.
            if (is_integer_ty(expr_ty.value())) {
                return expr_ty.value();
            } else {
                report(ctx, SimpleError("cannot apply to non-integer",
                                        std::nullopt, unary->span()));
                return std::nullopt;
            }
        } else if (unary->op().kind() == UnaryExpressionOpKind::BitInv ||
                   unary->op().kind() == UnaryExpressionOpKind::Neg) {
            if (is_integer_ty(expr_ty.value())) {
                return expr_ty.value();
            } else {
                report(ctx, SimpleError("cannot apply to non-integer",
                                        std::nullopt, unary->span()));
                return std::nullopt;
            }
        } else {
            if (expr_ty.value()->kind() == TypeKind::Pointer) {
                return expr_ty;
            } else if (is_integer_ty(expr_ty.value())) {
                return expr_ty;
            } else {
                report(ctx, SimpleError("cannot dereference non-pointer",
                                        std::nullopt, unary->span()));
                return std::nullopt;
            }
        }
    } else if (expr->kind() == ExpressionKind::Infix) {
        auto infix = std::static_pointer_cast<InfixExpression>(expr);
        auto lhs_ty = tyassert(ctx, infix->lhs());
        if (!lhs_ty) {
            return std::nullopt;
        }
        auto rhs_ty = tyassert(ctx, infix->rhs());
        if (!rhs_ty) {
            return std::nullopt;
        }
        if (lhs_ty.value()->kind() == TypeKind::Pointer) {
            if (infix->op().kind() == InfixExpressionOpKind::Add ||
                infix->op().kind() == InfixExpressionOpKind::AddAssign ||
                infix->op().kind() == InfixExpressionOpKind::Sub ||
                infix->op().kind() == InfixExpressionOpKind::SubAssign) {
                if (is_integer_ty(rhs_ty.value())) {
                    return lhs_ty;
                } else {
                    report(ctx,
                           SimpleError("invalid indix operator application",
                                       std::nullopt, infix->span()));
                    return std::nullopt;
                }
            } else {
                report(ctx, SimpleError("invalid indix operator application",
                                        std::nullopt, infix->span()));
                return std::nullopt;
            }
        } else {
            if (is_integer_ty(lhs_ty.value()) &&
                is_integer_ty(rhs_ty.value())) {
                return lhs_ty;
            } else {
                report(ctx, SimpleError("invalid indix operator application",
                                        std::nullopt, infix->span()));
                return std::nullopt;
            }
        }
    } else if (expr->kind() == ExpressionKind::Postfix) {
        auto postfix = std::static_pointer_cast<PostfixExpression>(expr);
        auto expr_ty = tyassert(ctx, postfix->expr());
        if (!expr_ty) {
            return std::nullopt;
        }
        if (expr_ty.value()->kind() == TypeKind::Pointer) {
            return expr_ty;
        } else if (is_integer_ty(expr_ty.value())) {
            return expr_ty;
        } else {
            report(ctx,
                   SimpleError(postfix->op().debug() + " cannot apply to this",
                               std::nullopt, postfix->span()));
            return std::nullopt;
        }
    } else if (expr->kind() == ExpressionKind::Access) {
        auto access = std::static_pointer_cast<AccessExpression>(expr);

        auto accessed_ty = tyassert(ctx, access->expr());
        if (!accessed_ty) {
            return std::nullopt;
        }

        if (accessed_ty.value()->kind() == TypeKind::Concrete) {
            auto concrete =
                std::static_pointer_cast<ConcreteType>(accessed_ty.value());
            if (concrete->specifier()->kind() == TypeSpecifierKind::Struct) {
                auto struct_ty = std::static_pointer_cast<StructTypeSpecifier>(
                    concrete->specifier());
                if (struct_ty->struct_kind() ==
                    StructTypeSpecifierKind::Named) {
                    auto body =
                        std::static_pointer_cast<NamedStructTypeSpecifier>(
                            struct_ty)
                            ->body();
                    if (!body) {
                        report(ctx, SimpleError(
                                        "cannot access to struct without body",
                                        std::nullopt, access->expr()->span()));
                        return std::nullopt;
                    }
                    for (const auto& vars_decl : body->decls()) {
                        for (const auto& decl : vars_decl.decls()) {
                            if (!decl->name()) {
                                continue;
                            }
                            if (decl->name()->name() ==
                                access->field().name()) {
                                return decl->type();
                            }
                        }
                    }
                    report(ctx,
                           SimpleError("no such field exist on this struct",
                                       std::nullopt, access->field().span()));
                    return std::nullopt;
                } else {
                    auto body =
                        std::static_pointer_cast<AnonymousStructTypeSpecifier>(
                            struct_ty)
                            ->body();
                    for (const auto& vars_decl : body.decls()) {
                        for (const auto& decl : vars_decl.decls()) {
                            if (!decl->name()) {
                                continue;
                            }
                            if (decl->name()->name() ==
                                access->field().name()) {
                                return decl->type();
                            }
                        }
                    }
                    report(ctx,
                           SimpleError("no such field exist on this struct",
                                       std::nullopt, access->field().span()));
                    return std::nullopt;
                }
            } else if (concrete->specifier()->kind() ==
                       TypeSpecifierKind::Union) {
                auto union_ty = std::static_pointer_cast<UnionTypeSpecifier>(
                    concrete->specifier());
                if (union_ty->union_kind() == UnionTypeSpecifierKind::Named) {
                    auto body =
                        std::static_pointer_cast<NamedUnionTypeSpecifier>(
                            union_ty)
                            ->body();
                    if (!body) {
                        report(ctx, SimpleError(
                                        "cannot access to union without body",
                                        std::nullopt, access->expr()->span()));
                        return std::nullopt;
                    }
                    for (const auto& vars_decl : body->decls()) {
                        for (const auto& decl : vars_decl.decls()) {
                            if (!decl->name()) {
                                continue;
                            }
                            if (decl->name()->name() ==
                                access->field().name()) {
                                return decl->type();
                            }
                        }
                    }
                    report(ctx,
                           SimpleError("no such field exist on this union",
                                       std::nullopt, access->field().span()));
                    return std::nullopt;
                } else {
                    auto body =
                        std::static_pointer_cast<AnonymousUnionTypeSpecifier>(
                            union_ty)
                            ->body();
                    for (const auto& vars_decl : body.decls()) {
                        for (const auto& decl : vars_decl.decls()) {
                            if (!decl->name()) {
                                continue;
                            }
                            if (decl->name()->name() ==
                                access->field().name()) {
                                return decl->type();
                            }
                        }
                    }
                    report(ctx,
                           SimpleError("no such field exist on this union",
                                       std::nullopt, access->field().span()));
                    return std::nullopt;
                }
            } else if (accessed_ty.value()->kind() == TypeKind::Pointer) {
                auto ptr =
                    std::static_pointer_cast<PointerType>(accessed_ty.value());
                if (ptr->kind() == TypeKind::Concrete) {
                }
            } else {
                goto failed_access;
            }
        } else if (accessed_ty.value()->kind() == TypeKind::Pointer) {
            auto ptr =
                std::static_pointer_cast<PointerType>(accessed_ty.value());
            if (ptr->kind() != TypeKind::Concrete) {
                goto failed_access;
            }
            auto concrete = std::static_pointer_cast<ConcreteType>(ptr);
            if (concrete->specifier()->kind() == TypeSpecifierKind::Struct) {
                auto struct_ty = std::static_pointer_cast<StructTypeSpecifier>(
                    concrete->specifier());
                if (struct_ty->struct_kind() ==
                    StructTypeSpecifierKind::Named) {
                    auto body =
                        std::static_pointer_cast<NamedStructTypeSpecifier>(
                            struct_ty)
                            ->body();
                    if (!body) {
                        report(ctx, SimpleError(
                                        "cannot access to struct without body",
                                        std::nullopt, access->expr()->span()));
                        return std::nullopt;
                    }
                    for (const auto& vars_decl : body->decls()) {
                        for (const auto& decl : vars_decl.decls()) {
                            if (!decl->name()) {
                                continue;
                            }
                            if (decl->name()->name() ==
                                access->field().name()) {
                                return decl->type();
                            }
                        }
                    }
                    report(ctx,
                           SimpleError("no such field exist on this struct",
                                       std::nullopt, access->field().span()));
                    return std::nullopt;
                } else {
                    auto body =
                        std::static_pointer_cast<AnonymousStructTypeSpecifier>(
                            struct_ty)
                            ->body();
                    for (const auto& vars_decl : body.decls()) {
                        for (const auto& decl : vars_decl.decls()) {
                            if (!decl->name()) {
                                continue;
                            }
                            if (decl->name()->name() ==
                                access->field().name()) {
                                return decl->type();
                            }
                        }
                    }
                    report(ctx,
                           SimpleError("no such field exist on this struct",
                                       std::nullopt, access->field().span()));
                    return std::nullopt;
                }
            } else if (concrete->specifier()->kind() ==
                       TypeSpecifierKind::Union) {
                auto union_ty = std::static_pointer_cast<UnionTypeSpecifier>(
                    concrete->specifier());
                if (union_ty->union_kind() == UnionTypeSpecifierKind::Named) {
                    auto body =
                        std::static_pointer_cast<NamedUnionTypeSpecifier>(
                            union_ty)
                            ->body();
                    if (!body) {
                        report(ctx, SimpleError(
                                        "cannot access to union without body",
                                        std::nullopt, access->expr()->span()));
                        return std::nullopt;
                    }
                    for (const auto& vars_decl : body->decls()) {
                        for (const auto& decl : vars_decl.decls()) {
                            if (!decl->name()) {
                                continue;
                            }
                            if (decl->name()->name() ==
                                access->field().name()) {
                                return decl->type();
                            }
                        }
                    }
                    report(ctx,
                           SimpleError("no such field exist on this union",
                                       std::nullopt, access->field().span()));
                    return std::nullopt;
                } else {
                    auto body =
                        std::static_pointer_cast<AnonymousUnionTypeSpecifier>(
                            union_ty)
                            ->body();
                    for (const auto& vars_decl : body.decls()) {
                        for (const auto& decl : vars_decl.decls()) {
                            if (!decl->name()) {
                                continue;
                            }
                            if (decl->name()->name() ==
                                access->field().name()) {
                                return decl->type();
                            }
                        }
                    }
                    report(ctx,
                           SimpleError("no such field exist on this union",
                                       std::nullopt, access->field().span()));
                    return std::nullopt;
                }
            } else if (accessed_ty.value()->kind() == TypeKind::Pointer) {
                auto ptr =
                    std::static_pointer_cast<PointerType>(accessed_ty.value());
                if (ptr->kind() == TypeKind::Concrete) {
                }
            } else {
                goto failed_access;
            }
        } else {
            goto failed_access;
        }

    failed_access:
        report(ctx,
               SimpleError("cannot access to non-struct or union expression",
                           std::nullopt, access->expr()->span()));
        return std::nullopt;
    } else if (expr->kind() == ExpressionKind::Indexing) {
        auto indexing = std::static_pointer_cast<IndexingExpression>(expr);

        auto index_ty = tyassert(ctx, indexing->index());
        if (!index_ty) {
            return std::nullopt;
        }

        if (!is_integer_ty(index_ty.value())) {
            report(ctx, SimpleError("cannot index with non-integer type",
                                    std::nullopt, indexing->index()->span()));
            return std::nullopt;
        }

        auto indexed_ty = tyassert(ctx, indexing->expr());
        if (!indexed_ty) {
            return std::nullopt;
        }

        if (indexed_ty.value()->kind() == TypeKind::Array) {
            auto array =
                std::static_pointer_cast<ArrayType>(indexed_ty.value());
            return array->of();
        } else if (indexed_ty.value()->kind() == TypeKind::Pointer) {
            auto ptr =
                std::static_pointer_cast<PointerType>(indexed_ty.value());
            return ptr->of();
        } else {
            report(ctx,
                   SimpleError("not indexable", "expected array or pointer",
                               indexing->expr()->span()));
            return std::nullopt;
        }
    } else if (expr->kind() == ExpressionKind::Calling) {
        auto call = std::static_pointer_cast<CallingExpression>(expr);
        auto func = tyassert(ctx, call->expr());
        if (!func) {
            return std::nullopt;
        }
        auto callable = is_callable(ctx, func.value());
        if (!callable) {
            report(ctx, SimpleError("not a callable", std::nullopt,
                                    call->expr()->span()));
            return std::nullopt;
        }
        auto params =
            std::static_pointer_cast<FunctionType>(callable.value())->params();
        if (params.size() != call->args().size()) {
            std::stringstream ss;
            ss << "expected " << params.size() << " but got "
               << call->args().size();
            report(ctx, SimpleError("incorrect number of arguments", ss.str(),
                                    expr->span()));
            return std::nullopt;
        }
        for (int i = 0; i < params.size(); i++) {
            auto arg_ty = tyassert(ctx, call->args()[i]);
            if (!arg_ty) {
                return std::nullopt;
            }
            if (!tycheck(ctx, arg_ty.value(), params[i].type())) {
                return std::nullopt;
            }
        }
        return callable.value()->ret_type();
    } else if (expr->kind() == ExpressionKind::Conditional) {
        auto e = std::static_pointer_cast<ConditionalExpression>(expr);

        auto cond_ty = tyassert(ctx, e->cond());
        if (!cond_ty) {
            return std::nullopt;
        }
        auto is_cond_scalar = is_scalar_type(ctx, cond_ty.value());
        if (!is_cond_scalar) {
            return std::nullopt;
        } else if (!is_cond_scalar.value()) {
            report(ctx, SimpleError("non-scalar type cannot come here",
                                    std::nullopt, e->cond()->span()));
            return std::nullopt;
        }

        auto then_ty = tyassert(ctx, e->then());
        if (!then_ty) {
            return std::nullopt;
        }

        auto otherwise_ty = tyassert(ctx, e->otherwise());
        if (!otherwise_ty) {
            return std::nullopt;
        }

        if (tycheck(ctx, then_ty.value(), otherwise_ty.value())) {
            return then_ty;
        } else {
            return std::nullopt;
        }
    } else if (expr->kind() == ExpressionKind::SizeofExpr) {
        // TODO: Follow C lang spec to decide return type of sizeof.
        auto spec = std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::Int, expr->span());
        return std::make_shared<ConcreteType>(spec, std::nullopt, std::nullopt);
    } else if (expr->kind() == ExpressionKind::SizeofType) {
        // TODO: Follow C lang spec to decide return type of sizeof.
        auto spec = std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::Int, expr->span());
        return std::make_shared<ConcreteType>(spec, std::nullopt, std::nullopt);
    } else if (expr->kind() == ExpressionKind::Cast) {
        auto cast = std::static_pointer_cast<CastExpression>(expr);
        return cast->type();
    } else if (expr->kind() == ExpressionKind::Integer) {
        // TODO: Follow C lang spec to convert integer.
        auto spec = std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::UnsignedLongLong, expr->span());
        return std::make_shared<ConcreteType>(spec, std::nullopt, std::nullopt);
    } else if (expr->kind() == ExpressionKind::Identifier) {
        auto id = std::static_pointer_cast<IdentifierExpression>(expr);
        try {
            return ctx.symbol_table().get(id->value());
        } catch (std::out_of_range e) {
            report(ctx, SimpleError("no such variable exist", std::nullopt,
                                    id->span()));
            return std::nullopt;
        }
    } else if (expr->kind() == ExpressionKind::String) {
        auto spec = std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::Char, expr->span());
        auto base =
            std::make_shared<ConcreteType>(spec, std::nullopt, std::nullopt);
        return std::make_shared<PointerType>(Star(expr->span()), std::nullopt,
                                             std::nullopt, base);
    } else if (expr->kind() == ExpressionKind::Character) {
        auto spec = std::make_shared<BuiltinTypeSpecifier>(
            BuiltinTypeSpecifierKind::Char, expr->span());
        return std::make_shared<ConcreteType>(spec, std::nullopt, std::nullopt);
    } else {
        auto inner = std::static_pointer_cast<SurroundedExpression>(expr);
        return tyassert(ctx, inner);
    }
}

bool tycheck(Context& ctx, const std::shared_ptr<Type>& lhs,
             const std::shared_ptr<Type>& rhs) {}

}  // namespace tinyc
