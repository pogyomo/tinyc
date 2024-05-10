#ifndef TINYC_PARSER_STMT_H_
#define TINYC_PARSER_STMT_H_

#include <memory>
#include <optional>
#include <sstream>
#include <variant>
#include <vector>

#include "decl.h"
#include "expr.h"
#include "node.h"

namespace tinyc {

enum class StatementKind {
    Labeled,
    Case,
    Default,
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

class LabeledStatementLabel : public Node {
public:
    LabeledStatementLabel(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class LabeledStatement : public Statement {
public:
    LabeledStatement(const LabeledStatementLabel& label, Colon colon,
                     const std::shared_ptr<Statement>& stmt)
        : label_(label), colon_(colon), stmt_(stmt) {}

    inline const LabeledStatementLabel& label() const { return label_; }

    inline const Colon& colon() const { return colon_; }

    inline const std::shared_ptr<Statement>& stmt() const { return stmt_; }

    inline StatementKind kind() const override {
        return StatementKind::Labeled;
    }

    inline Span span() const override {
        return concat_spans({label_.span(), colon_.span(), stmt_->span()});
    }

    inline std::string debug() const override {
        return label_.debug() + colon_.debug() + " " + stmt_->debug();
    }

private:
    const LabeledStatementLabel label_;
    const Colon colon_;
    const std::shared_ptr<Statement> stmt_;
};

class CaseStatement : public Statement {
public:
    CaseStatement(Case case_kw, const std::shared_ptr<Expression> expr,
                  Colon colon, const std::shared_ptr<Statement>& stmt)
        : case_kw_(case_kw), expr_(expr), colon_(colon), stmt_(stmt) {}

    inline const Case& case_kw() const { return case_kw_; }

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    inline const Colon& colon() const { return colon_; }

    inline const std::shared_ptr<Statement>& stmt() const { return stmt_; }

    inline StatementKind kind() const override { return StatementKind::Case; }

    inline Span span() const override {
        return concat_spans(
            {case_kw_.span(), expr_->span(), colon_.span(), stmt_->span()});
    }

    std::string debug() const override {
        auto case_kw = case_kw_.debug();
        auto expr = expr_->debug();
        auto colon = colon_.debug();
        auto stmt = stmt_->debug();
        return case_kw + " " + expr + colon + " " + stmt;
    }

private:
    const Case case_kw_;
    const std::shared_ptr<Expression> expr_;
    const Colon colon_;
    const std::shared_ptr<Statement> stmt_;
};

class DefaultStatement : public Statement {
public:
    DefaultStatement(Default default_kw, Colon colon,
                     const std::shared_ptr<Statement>& stmt)
        : default_kw_(default_kw), colon_(colon), stmt_(stmt) {}

    inline const Default& default_kw() const { return default_kw_; }

    inline const Colon& colon() const { return colon_; }

    inline const std::shared_ptr<Statement>& stmt() const { return stmt_; }

    inline StatementKind kind() const override {
        return StatementKind::Default;
    }

    inline Span span() const override {
        return concat_spans({default_kw_.span(), colon_.span(), stmt_->span()});
    }

    std::string debug() const override {
        auto default_kw = default_kw_.debug();
        auto colon = colon_.debug();
        auto stmt = stmt_->debug();
        return default_kw + colon + " " + stmt;
    }

private:
    const Default default_kw_;
    const Colon colon_;
    const std::shared_ptr<Statement> stmt_;
};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(Semicolon semicolon)
        : expr_(std::nullopt), semicolon_(semicolon) {}

    ExpressionStatement(const std::shared_ptr<Expression>& expr,
                        Semicolon semicolon)
        : expr_(expr), semicolon_(semicolon) {}

    inline const std::optional<std::shared_ptr<Expression>>& expr() const {
        return expr_;
    }

    inline const Semicolon& semicolon() const { return semicolon_; }

    inline StatementKind kind() const override {
        return StatementKind::Expression;
    }

    Span span() const override {
        if (expr_.has_value()) {
            return concat_spans({expr_.value()->span(), semicolon_.span()});
        } else {
            return semicolon_.span();
        }
    }

    std::string debug() const override {
        if (expr_.has_value()) {
            return expr_.value()->debug() + semicolon_.debug();
        } else {
            return semicolon_.debug();
        }
    }

private:
    const std::optional<std::shared_ptr<Expression>> expr_;
    const Semicolon semicolon_;
};

enum class BlockStatementItemKind {
    Statement,
    Declaration,
};

class BlockStatementItem : public Node {
public:
    virtual ~BlockStatementItem() {}
    virtual BlockStatementItemKind kind() const = 0;
};

class BlockStatementStatementItem : public BlockStatementItem {
public:
    BlockStatementStatementItem(const std::shared_ptr<Statement>& stmt)
        : stmt_(stmt) {}

    inline const std::shared_ptr<Statement>& stmt() const { return stmt_; }

    inline BlockStatementItemKind kind() const override {
        return BlockStatementItemKind::Statement;
    }

    inline Span span() const override { return stmt_->span(); }

    inline std::string debug() const override { return stmt_->debug(); }

private:
    const std::shared_ptr<Statement> stmt_;
};

class BlockStatementDeclarationItem : public BlockStatementItem {
public:
    BlockStatementDeclarationItem(const std::shared_ptr<Declaration>& decl)
        : decl_(decl) {}

    inline const std::shared_ptr<Declaration>& decl() const { return decl_; }

    inline BlockStatementItemKind kind() const override {
        return BlockStatementItemKind::Declaration;
    }

    inline Span span() const override { return decl_->span(); }

    inline std::string debug() const override { return decl_->debug(); }

private:
    const std::shared_ptr<Declaration> decl_;
};

class BlockStatement : public Statement {
public:
    BlockStatement(
        LCurly lcurly,
        const std::vector<std::shared_ptr<BlockStatementItem>>& items,
        RCurly rcurly)
        : lcurly_(lcurly), items_(items), rcurly_(rcurly) {}

    const LCurly& lcurly() const { return lcurly_; };

    const std::vector<std::shared_ptr<BlockStatementItem>>& items() const {
        return items_;
    }

    const RCurly& rcurly() const { return rcurly_; }

    inline StatementKind kind() const override { return StatementKind::Block; }

    Span span() const override {
        std::vector<Span> spans;
        spans.emplace_back(lcurly_.span());
        for (const auto& item : items_) {
            spans.emplace_back(item->span());
        }
        spans.emplace_back(rcurly_.span());
        return concat_spans(spans);
    }

    std::string debug() const override {
        std::stringstream ss;
        ss << lcurly_.debug();
        for (const auto& item : items_) {
            ss << " " << item->debug();
        }
        ss << " " << rcurly_.debug();
        return ss.str();
    }

private:
    const LCurly lcurly_;
    const std::vector<std::shared_ptr<BlockStatementItem>> items_;
    const RCurly rcurly_;
};

class IfStatementIfBody : public Node {
public:
    IfStatementIfBody(If if_kw, LParen lparen,
                      const std::shared_ptr<Expression>& cond, RParen rparen,
                      const std::shared_ptr<Statement> body)
        : if_kw_(if_kw),
          lparen_(lparen),
          cond_(cond),
          rparen_(rparen),
          body_(body) {}

    inline const If& if_kw() const { return if_kw_; }

    inline const LParen& lparen() const { return lparen_; }

    inline const std::shared_ptr<Expression>& cond() const { return cond_; }

    inline const RParen& rparen() const { return rparen_; }

    inline const std::shared_ptr<Statement>& body() const { return body_; }

    inline Span span() const override {
        return concat_spans({if_kw_.span(), lparen_.span(), cond_->span(),
                             rparen_.span(), body_->span()});
    }

    std::string debug() const override {
        auto if_kw = if_kw_.debug();
        auto lparen = lparen_.debug();
        auto cond = cond_->debug();
        auto rparen = rparen_.debug();
        auto body = body_->debug();
        return if_kw + " " + lparen + cond + rparen + " " + body;
    }

private:
    const If if_kw_;
    const LParen lparen_;
    const std::shared_ptr<Expression> cond_;
    const RParen rparen_;
    const std::shared_ptr<Statement> body_;
};

class IfStatementElseBody : public Node {
public:
    IfStatementElseBody(Else else_kw, const std::shared_ptr<Statement>& body)
        : else_kw_(else_kw), body_(body) {}

    inline const Else& else_kw() { return else_kw_; }

    inline const std::shared_ptr<Statement>& body() { return body_; }

    inline Span span() const override {
        return concat_spans({else_kw_.span(), body_->span()});
    }

    inline std::string debug() const override {
        return else_kw_.debug() + " " + body_->debug();
    }

private:
    const Else else_kw_;
    const std::shared_ptr<Statement> body_;
};

class IfStatement : public Statement {
public:
    IfStatement(IfStatementIfBody if_body)
        : if_body_(if_body), else_body_(std::nullopt) {}

    IfStatement(IfStatementIfBody if_body, IfStatementElseBody else_body)
        : if_body_(if_body), else_body_(else_body) {}

    inline const IfStatementIfBody& if_body() const { return if_body_; }

    inline const std::optional<IfStatementElseBody>& else_body() const {
        return else_body_;
    }

    inline StatementKind kind() const override { return StatementKind::If; }

    Span span() const override {
        std::vector<Span> spans;
        spans.emplace_back(if_body_.span());
        if (else_body_.has_value()) {
            spans.emplace_back(else_body_->span());
        }
        return concat_spans(spans);
    }

    std::string debug() const override {
        if (else_body_.has_value()) {
            return if_body_.debug() + else_body_->debug();
        } else {
            return if_body_.debug();
        }
    }

private:
    const IfStatementIfBody if_body_;
    const std::optional<IfStatementElseBody> else_body_;
};

class SwitchStatement : public Statement {
public:
    SwitchStatement(Switch switch_kw, LParen lparen,
                    const std::shared_ptr<Expression>& expr, RParen rparen,
                    const std::shared_ptr<Statement>& stmt)
        : switch_kw_(switch_kw),
          lparen_(lparen),
          expr_(expr),
          rparen_(rparen),
          stmt_(stmt) {}

    inline const Switch& switch_kw() const { return switch_kw_; }

    inline const LParen& lparen() const { return lparen_; }

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    inline const RParen& rparen() const { return rparen_; }

    inline const std::shared_ptr<Statement>& stmt() const { return stmt_; }

    inline StatementKind kind() const override { return StatementKind::Switch; }

    Span span() const override {
        return concat_spans({switch_kw_.span(), lparen_.span(), expr_->span(),
                             rparen_.span(), stmt_->span()});
    }

    std::string debug() const override {
        auto switch_kw = switch_kw_.debug();
        auto lparen = lparen_.debug();
        auto expr = expr_->debug();
        auto rparen = rparen_.debug();
        auto stmt = stmt_->debug();
        return switch_kw + " " + lparen + expr + rparen + " " + stmt;
    }

private:
    const Switch switch_kw_;
    const LParen lparen_;
    const std::shared_ptr<Expression> expr_;
    const RParen rparen_;
    const std::shared_ptr<Statement> stmt_;
};

class WhileStatement : public Statement {
public:
    WhileStatement(While while_kw, LParen lparen,
                   const std::shared_ptr<Expression>& expr, RParen rparen,
                   const std::shared_ptr<Statement>& stmt)
        : while_kw_(while_kw),
          lparen_(lparen),
          expr_(expr),
          rparen_(rparen),
          stmt_(stmt) {}

    inline const While& while_kw() const { return while_kw_; }

    inline const LParen& lparen() const { return lparen_; }

    inline const std::shared_ptr<Expression>& expr() const { return expr_; }

    inline const RParen& rparen() const { return rparen_; }

    inline const std::shared_ptr<Statement>& stmt() const { return stmt_; }

    inline StatementKind kind() const override { return StatementKind::While; }

    Span span() const override {
        return concat_spans({while_kw_.span(), lparen_.span(), expr_->span(),
                             rparen_.span(), stmt_->span()});
    }

    std::string debug() const override {
        auto while_kw = while_kw_.debug();
        auto lparen = lparen_.debug();
        auto expr = expr_->debug();
        auto rparen = rparen_.debug();
        auto stmt = stmt_->debug();
        return while_kw + " " + lparen + expr + rparen + " " + stmt;
    }

private:
    const While while_kw_;
    const LParen lparen_;
    const std::shared_ptr<Expression> expr_;
    const RParen rparen_;
    const std::shared_ptr<Statement> stmt_;
};

class DoWhileStatement : public Statement {
public:
    DoWhileStatement(Do do_kw, const std::shared_ptr<Statement>& stmt,
                     While while_kw, LParen lparen,
                     const std::shared_ptr<Expression>& cond, RParen rparen,
                     Semicolon semicolon)
        : do_kw_(do_kw),
          stmt_(stmt),
          while_kw_(while_kw),
          lparen_(lparen),
          cond_(cond),
          rparen_(rparen),
          semicolon_(semicolon) {}

    inline const Do& do_kw() const { return do_kw_; }

    inline const std::shared_ptr<Statement>& stmt() const { return stmt_; }

    inline const While& while_kw() const { return while_kw_; }

    inline const LParen& lparen() const { return lparen_; }

    inline const std::shared_ptr<Expression>& cond() const { return cond_; }

    inline const RParen& rparen() const { return rparen_; }

    inline const Semicolon& semicolon() const { return semicolon_; }

    inline StatementKind kind() const override {
        return StatementKind::DoWhile;
    }

    Span span() const override {
        return concat_spans({do_kw_.span(), stmt_->span(), while_kw_.span(),
                             lparen_.span(), cond_->span(), rparen_.span(),
                             semicolon_.span()});
    }

    std::string debug() const override {
        auto do_kw = do_kw_.debug();
        auto stmt = stmt_->debug();
        auto while_kw = while_kw_.debug();
        auto lparen = lparen_.debug();
        auto cond = cond_->debug();
        auto rparen = rparen_.debug();
        auto semicolon = semicolon_.debug();
        return do_kw + " " + stmt + " " + while_kw + " " + lparen + cond +
               rparen + semicolon;
    }

private:
    const Do do_kw_;
    const std::shared_ptr<Statement> stmt_;
    const While while_kw_;
    const LParen lparen_;
    const std::shared_ptr<Expression> cond_;
    const RParen rparen_;
    const Semicolon semicolon_;
};

enum class ForStatementInitKind {
    Decl,
    Expr,
};

class ForStatementInit : public Node {
public:
    virtual ~ForStatementInit() {}
    virtual ForStatementInitKind kind() const = 0;
};

class ForStatementInitDecl : public ForStatementInit {
public:
    ForStatementInitDecl(const std::shared_ptr<VariablesDeclaration>& decl)
        : decl_(decl) {}

    inline const std::shared_ptr<VariablesDeclaration>& decl() const {
        return decl_;
    }

    inline ForStatementInitKind kind() const override {
        return ForStatementInitKind::Decl;
    }

    inline Span span() const override { return decl_->span(); }

    inline std::string debug() const override { return decl_->debug(); }

private:
    const std::shared_ptr<VariablesDeclaration> decl_;
};

class ForStatementInitExpr : public ForStatementInit {
public:
    ForStatementInitExpr(Semicolon semicolon)
        : init_(std::nullopt), semicolon_(semicolon) {}

    ForStatementInitExpr(std::shared_ptr<Expression> init, Semicolon semicolon)
        : init_(init), semicolon_(semicolon) {}

    inline const std::optional<std::shared_ptr<Expression>>& init() const {
        return init_;
    }

    inline const Semicolon& semicolon() const { return semicolon_; }

    inline ForStatementInitKind kind() const override {
        return ForStatementInitKind::Expr;
    }

    Span span() const override {
        if (init_.has_value()) {
            return concat_spans({init_.value()->span(), semicolon_.span()});
        } else {
            return semicolon_.span();
        }
    }

    std::string debug() const override {
        if (init_.has_value()) {
            return init_.value()->debug() + semicolon_.debug();
        } else {
            return semicolon_.debug();
        }
    }

private:
    const std::optional<std::shared_ptr<Expression>> init_;
    const Semicolon semicolon_;
};

class ForStatementCond : public Node {
public:
    ForStatementCond(Semicolon semicolon)
        : cond_(std::nullopt), semicolon_(semicolon) {}

    ForStatementCond(std::shared_ptr<Expression> cond, Semicolon semicolon)
        : cond_(cond), semicolon_(semicolon) {}

    inline const std::optional<std::shared_ptr<Expression>>& cond() const {
        return cond_;
    }

    inline const Semicolon& semicolon() const { return semicolon_; }

    Span span() const override {
        if (cond_.has_value()) {
            return concat_spans({cond_.value()->span(), semicolon_.span()});
        } else {
            return semicolon_.span();
        }
    }

    std::string debug() const override {
        if (cond_.has_value()) {
            return cond_.value()->debug() + semicolon_.debug();
        } else {
            return semicolon_.debug();
        }
    }

private:
    const std::optional<std::shared_ptr<Expression>> cond_;
    const Semicolon semicolon_;
};

class ForStatement : public Statement {
public:
    ForStatement(For for_kw, LParen lparen,
                 const std::shared_ptr<ForStatementInit>& init,
                 const ForStatementCond& cond, RParen rparen,
                 const std::shared_ptr<Statement> body)
        : for_kw_(for_kw),
          lparen_(lparen),
          init_(init),
          cond_(cond),
          update_(std::nullopt),
          rparen_(rparen),
          body_(body) {}

    ForStatement(For for_kw, LParen lparen,
                 const std::shared_ptr<ForStatementInit>& init,
                 const ForStatementCond& cond,
                 const std::shared_ptr<Expression>& update, RParen rparen,
                 const std::shared_ptr<Statement> body)
        : for_kw_(for_kw),
          lparen_(lparen),
          init_(init),
          cond_(cond),
          update_(update),
          rparen_(rparen),
          body_(body) {}

    inline const For for_kw() const { return for_kw_; }

    inline const LParen lparen() const { return lparen_; }

    inline const std::shared_ptr<ForStatementInit>& init() const {
        return init_;
    }

    inline const ForStatementCond cond() const { return cond_; }

    inline const std::optional<std::shared_ptr<Expression>> update() const {
        return update_;
    }

    inline const RParen rparen() const { return rparen_; }

    inline const std::shared_ptr<Statement> body() const { return body_; }

    inline StatementKind kind() const override { return StatementKind::For; }

    Span span() const override {
        std::vector<Span> spans;
        spans.emplace_back(for_kw_.span());
        spans.emplace_back(lparen_.span());
        spans.emplace_back(init_->span());
        spans.emplace_back(cond_.span());
        if (update_.has_value()) {
            spans.emplace_back(update_.value()->span());
        }
        spans.emplace_back(rparen_.span());
        spans.emplace_back(body_->span());
        return concat_spans(spans);
    }

    std::string debug() const override {
        std::stringstream ss;
        ss << for_kw_.debug() << lparen_.debug();
        ss << init_->debug() << " " << cond_.debug() << " ";
        if (update_.has_value()) {
            ss << update_.value()->debug();
        }
        ss << rparen_.debug() << " " << body_->debug();
        return ss.str();
    }

private:
    const For for_kw_;
    const LParen lparen_;
    const std::shared_ptr<ForStatementInit> init_;
    const ForStatementCond cond_;
    const std::optional<std::shared_ptr<Expression>> update_;
    const RParen rparen_;
    const std::shared_ptr<Statement> body_;
};

class GotoStatementLabel : public Node {
public:
    GotoStatementLabel(const std::string& name, Span span)
        : name_(name), span_(span) {}

    inline const std::string& name() const { return name_; }

    inline Span span() const override { return span_; }

    inline std::string debug() const override { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class GotoStatement : public Statement {
public:
    GotoStatement(Goto goto_kw, GotoStatementLabel label, Semicolon semicolon)
        : goto_kw_(goto_kw), label_(label), semicolon_(semicolon) {}

    inline const Goto goto_kw() const { return goto_kw_; }

    inline const GotoStatementLabel label() const { return label_; }

    inline const Semicolon semicolon() const { return semicolon_; }

    inline StatementKind kind() const override { return StatementKind::Goto; }

    inline Span span() const override {
        return concat_spans(
            {goto_kw_.span(), label_.span(), semicolon_.span()});
    }

    inline std::string debug() const override {
        return goto_kw_.debug() + " " + label_.debug() + semicolon_.debug();
    }

private:
    const Goto goto_kw_;
    const GotoStatementLabel label_;
    const Semicolon semicolon_;
};

class ContinueStatement : public Statement {
public:
    ContinueStatement(Continue continue_kw, Semicolon semicolon)
        : continue_kw_(continue_kw), semicolon_(semicolon) {}

    inline const Continue continue_kw() const { return continue_kw_; }

    inline const Semicolon semicolon() const { return semicolon_; }

    inline StatementKind kind() const override {
        return StatementKind::Continue;
    }

    inline Span span() const override {
        return concat_spans({continue_kw_.span(), semicolon_.span()});
    }

    inline std::string debug() const override {
        return continue_kw_.debug() + semicolon_.debug();
    }

private:
    const Continue continue_kw_;
    const Semicolon semicolon_;
};

class BreakStatement : public Statement {
public:
    BreakStatement(Break break_kw, Semicolon semicolon)
        : break_kw_(break_kw), semicolon_(semicolon) {}

    inline const Break break_kw() const { return break_kw_; }

    inline const Semicolon semicolon() const { return semicolon_; }

    inline StatementKind kind() const override { return StatementKind::Break; }

    inline Span span() const override {
        return concat_spans({break_kw_.span(), semicolon_.span()});
    }

    inline std::string debug() const override {
        return break_kw_.debug() + semicolon_.debug();
    }

private:
    const Break break_kw_;
    const Semicolon semicolon_;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(Return return_kw, const std::shared_ptr<Expression>& expr,
                    Semicolon semicolon)
        : return_kw_(return_kw), expr_(expr), semicolon_(semicolon) {}

    inline const Return return_kw() const { return return_kw_; }

    inline const std::shared_ptr<Expression> expr() const { return expr_; }

    inline const Semicolon semicolon() const { return semicolon_; }

    inline StatementKind kind() const override { return StatementKind::Return; }

    inline Span span() const override {
        return concat_spans(
            {return_kw_.span(), expr_->span(), semicolon_.span()});
    }

    inline std::string debug() const override {
        return return_kw_.debug() + " " + expr_->debug() + semicolon_.debug();
    }

private:
    const Return return_kw_;
    const std::shared_ptr<Expression> expr_;
    const Semicolon semicolon_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_STMT_H_
