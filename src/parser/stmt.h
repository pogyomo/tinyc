#ifndef TINYC_PARSER_STMT_H_
#define TINYC_PARSER_STMT_H_

#include <climits>
#include <memory>
#include <optional>
#include <sstream>
#include <vector>

#include "../span.h"
#include "expr.h"

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

class Statement {
public:
    virtual ~Statement() {}
    virtual StatementKind kind() const = 0;
    virtual Span span() const = 0;
    virtual std::string debug() const = 0;
};

class LabeledStatementColon {
public:
    LabeledStatementColon(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ":"; }

private:
    const Span span_;
};

class LabeledStatementLabel {
public:
    LabeledStatementLabel(const std::string& name, Span span)
        : span_(span), name_(name) {}

    const std::string& name() const { return name_; }

    Span span() const { return span_; }

    std::string debug() const { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class LabeledStatement : public Statement {
public:
    LabeledStatement(LabeledStatementLabel label, LabeledStatementColon colon,
                     const std::shared_ptr<Statement> stmt)
        : label_(label), colon_(colon), stmt_(stmt) {}

    const LabeledStatementLabel& label() const { return label_; }

    const LabeledStatementColon& colon() const { return colon_; }

    const std::shared_ptr<Statement>& stmt() const { return stmt_; }

    StatementKind kind() const override { return StatementKind::Labeled; }

    Span span() const override {
        return Span(label_.span().start(), stmt_->span().end(),
                    label_.span().id());
    }

    std::string debug() const override {
        return label_.debug() + " " + colon_.debug() + " " + stmt_->debug();
    }

private:
    const LabeledStatementLabel label_;
    const LabeledStatementColon colon_;
    const std::shared_ptr<Statement> stmt_;
};

class CaseStatementCaseKeyword {
public:
    CaseStatementCaseKeyword(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "case"; }

private:
    const Span span_;
};

class CaseStatementColon {
public:
    CaseStatementColon(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ":"; }

private:
    const Span span_;
};

class CaseStatement : public Statement {
public:
    CaseStatement(CaseStatementCaseKeyword case_keyword,
                  const std::shared_ptr<Expression> expr,
                  CaseStatementColon colon,
                  const std::shared_ptr<Statement> stmt)
        : case_keyword_(case_keyword),
          expr_(expr),
          colon_(colon),
          stmt_(stmt) {}

    const CaseStatementCaseKeyword& label() const { return case_keyword_; }

    const std::shared_ptr<Expression>& expr() const { return expr_; };

    const CaseStatementColon& colon() const { return colon_; }

    const std::shared_ptr<Statement>& stmt() const { return stmt_; }

    StatementKind kind() const override { return StatementKind::Case; }

    Span span() const override {
        return Span(case_keyword_.span().start(), stmt_->span().end(),
                    case_keyword_.span().id());
    }

    std::string debug() const override {
        return case_keyword_.debug() + " " + expr_->debug() + colon_.debug() +
               " " + stmt_->debug();
    }

private:
    const CaseStatementCaseKeyword case_keyword_;
    const std::shared_ptr<Expression> expr_;
    const CaseStatementColon colon_;
    const std::shared_ptr<Statement> stmt_;
};

class DefaultStatementDefaultKeyword {
public:
    DefaultStatementDefaultKeyword(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "default"; }

private:
    const Span span_;
};

class DefaultStatementColon {
public:
    DefaultStatementColon(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ":"; }

private:
    const Span span_;
};

class DefaultStatement : public Statement {
public:
    DefaultStatement(DefaultStatementDefaultKeyword keyword,
                     DefaultStatementColon colon,
                     const std::shared_ptr<Statement> stmt)
        : default_keyword_(keyword), colon_(colon), stmt_(stmt) {}

    const DefaultStatementDefaultKeyword& label() const {
        return default_keyword_;
    }

    const DefaultStatementColon& colon() const { return colon_; }

    const std::shared_ptr<Statement>& stmt() const { return stmt_; }

    StatementKind kind() const override { return StatementKind::Default; }

    Span span() const override {
        return Span(default_keyword_.span().start(), stmt_->span().end(),
                    default_keyword_.span().id());
    }

    std::string debug() const override {
        return default_keyword_.debug() + colon_.debug() + " " + stmt_->debug();
    }

private:
    const DefaultStatementDefaultKeyword default_keyword_;
    const DefaultStatementColon colon_;
    const std::shared_ptr<Statement> stmt_;
};

class EmptyStatementSemicolon {
public:
    EmptyStatementSemicolon(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ";"; }

private:
    const Span span_;
};

// Just `;`.
class EmptyStatement : public Statement {
public:
    EmptyStatement(EmptyStatementSemicolon semicolon) : semicolon_(semicolon) {}

    const EmptyStatementSemicolon& semicolon() const { return semicolon_; }

    StatementKind kind() const override { return StatementKind::Empty; }

    Span span() const override { return semicolon_.span(); }

    std::string debug() const override { return semicolon_.debug(); }

private:
    const EmptyStatementSemicolon semicolon_;
};

class ExpressionStatementSemicolon {
public:
    ExpressionStatementSemicolon(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ";"; }

private:
    const Span span_;
};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(const std::shared_ptr<Expression>& expr,
                        ExpressionStatementSemicolon semicolon)
        : expr_(expr), semicolon_(semicolon) {}

    const std::shared_ptr<Expression>& expr() const { return expr_; }

    const ExpressionStatementSemicolon& semicolon() const { return semicolon_; }

    StatementKind kind() const override { return StatementKind::Expression; }

    Span span() const override {
        return Span(expr_->span().start(), semicolon_.span().end(),
                    expr_->span().id());
    }

    std::string debug() const override {
        return expr_->debug() + semicolon_.debug();
    }

private:
    const std::shared_ptr<Expression> expr_;
    const ExpressionStatementSemicolon semicolon_;
};

class BlockStatementLCurly {
public:
    BlockStatementLCurly(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "{"; }

private:
    const Span span_;
};

class BlockStatementRCurly {
public:
    BlockStatementRCurly(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "}"; }

private:
    const Span span_;
};

// TODO: Add decls.
class BlockStatement : public Statement {
public:
    BlockStatement(BlockStatementLCurly lcurly,
                   const std::vector<std::shared_ptr<Statement>> stmts,
                   BlockStatementRCurly rcurly)
        : lcurly_(lcurly), stmts_(stmts), rcurly_(rcurly) {}

    const std::vector<std::shared_ptr<Statement>>& stmts() const {
        return stmts_;
    };

    StatementKind kind() const override { return StatementKind::Block; }

    Span span() const override {
        return Span(lcurly_.span().start(), rcurly_.span().end(),
                    lcurly_.span().id());
    }

    std::string debug() const override {
        std::stringstream ss;
        ss << lcurly_.debug();
        for (const auto& stmt : stmts_) {
            ss << " " << stmt->debug();
        }
        ss << " " << rcurly_.debug();
        return ss.str();
    }

private:
    const BlockStatementLCurly lcurly_;
    const std::vector<std::shared_ptr<Statement>> stmts_;
    const BlockStatementRCurly rcurly_;
};

class IfStatementIfKeyword {
public:
    IfStatementIfKeyword(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "if"; }

private:
    const Span span_;
};

class IfStatementElseKeyword {
public:
    IfStatementElseKeyword(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "else"; }

private:
    const Span span_;
};

class IfStatementLParen {
public:
    IfStatementLParen(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "("; }

private:
    const Span span_;
};

class IfStatementRParen {
public:
    IfStatementRParen(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ")"; }

private:
    const Span span_;
};

class IfStatementElseBody {
public:
    IfStatementElseBody(IfStatementElseKeyword else_keyword,
                        const std::shared_ptr<Statement>& otherwise)
        : else_keyword_(else_keyword), otherwise_(otherwise) {}

    const IfStatementElseKeyword& else_keyword() const { return else_keyword_; }

    const std::shared_ptr<Statement>& otherwise() const { return otherwise_; }

    Span span() const {
        return Span(else_keyword_.span().start(), otherwise_->span().end(),
                    else_keyword_.span().id());
    }

    std::string debug() const {
        return else_keyword_.debug() + " " + otherwise_->debug();
    }

private:
    const IfStatementElseKeyword else_keyword_;
    const std::shared_ptr<Statement> otherwise_;
};

class IfStatement : public Statement {
public:
    IfStatement(IfStatementIfKeyword if_keyword, IfStatementLParen lparen,
                const std::shared_ptr<Expression>& cond,
                IfStatementRParen rparen,
                const std::shared_ptr<Statement>& then,
                const std::optional<IfStatementElseBody>& else_body)
        : if_keyword_(if_keyword),
          lparen_(lparen),
          cond_(cond),
          rparen_(rparen),
          then_(then),
          else_body_(else_body) {}

    const IfStatementIfKeyword& if_keyword() const { return if_keyword_; }

    const IfStatementLParen& lparen() const { return lparen_; }

    const std::shared_ptr<Expression>& cond() const { return cond_; }

    const IfStatementRParen& rparen() const { return rparen_; }

    const std::shared_ptr<Statement>& then() const { return then_; }

    const std::optional<IfStatementElseBody>& else_body() const {
        return else_body_;
    }

    StatementKind kind() const { return StatementKind::If; }

    Span span() const {
        if (else_body_.has_value()) {
            return Span(if_keyword_.span().start(), else_body_->span().end(),
                        if_keyword_.span().id());
        } else {
            return Span(if_keyword_.span().start(), then_->span().end(),
                        if_keyword_.span().id());
        }
    }

    std::string debug() const {
        auto if_keyword = if_keyword_.debug();
        auto lparen = lparen_.debug();
        auto cond = cond_->debug();
        auto rparen = rparen_.debug();
        auto then = then_->debug();
        auto else_body = else_body_.has_value() ? else_body_->debug() : "";
        return if_keyword + " " + lparen + cond + rparen + " " + then + " " +
               else_body;
    }

private:
    const IfStatementIfKeyword if_keyword_;
    const IfStatementLParen lparen_;
    const std::shared_ptr<Expression> cond_;
    const IfStatementRParen rparen_;
    const std::shared_ptr<Statement> then_;
    const std::optional<IfStatementElseBody> else_body_;
};

class SwitchStatementSwitchKeyword {
public:
    SwitchStatementSwitchKeyword(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "switch"; }

private:
    const Span span_;
};

class SwitchStatementLParen {
public:
    SwitchStatementLParen(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "("; }

private:
    const Span span_;
};

class SwitchStatementRParen {
public:
    SwitchStatementRParen(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ")"; }

private:
    const Span span_;
};

class SwitchStatement : public Statement {
public:
    SwitchStatement(SwitchStatementSwitchKeyword switch_keyword,
                    SwitchStatementLParen lparen,
                    const std::shared_ptr<Expression>& comp,
                    SwitchStatementRParen rparen,
                    const std::shared_ptr<Statement>& body)
        : switch_keyword_(switch_keyword),
          lparen_(lparen),
          comp_(comp),
          rparen_(rparen),
          body_(body) {}

    const SwitchStatementSwitchKeyword& switch_keyword() const {
        return switch_keyword_;
    }

    const SwitchStatementLParen& lparen() const { return lparen_; }

    const std::shared_ptr<Expression>& comp() const { return comp_; }

    const SwitchStatementRParen& rparen() const { return rparen_; }

    const std::shared_ptr<Statement>& body() const { return body_; }

    StatementKind kind() const { return StatementKind::Switch; }

    Span span() const {
        return Span(switch_keyword_.span().start(), body_->span().end(),
                    switch_keyword_.span().id());
    }

    std::string debug() const {
        auto switch_keyword = switch_keyword_.debug();
        auto lparen = lparen_.debug();
        auto comp = comp_->debug();
        auto rparen = rparen_.debug();
        auto body = body_->debug();
        return switch_keyword + " " + lparen + comp + rparen + " " + body;
    }

private:
    const SwitchStatementSwitchKeyword switch_keyword_;
    const SwitchStatementLParen lparen_;
    const std::shared_ptr<Expression> comp_;
    const SwitchStatementRParen rparen_;
    const std::shared_ptr<Statement> body_;
};

class WhileStatementWhileKeyword {
public:
    WhileStatementWhileKeyword(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "while"; }

private:
    const Span span_;
};

class WhileStatementLParen {
public:
    WhileStatementLParen(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "("; }

private:
    const Span span_;
};

class WhileStatementRParen {
public:
    WhileStatementRParen(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ")"; }

private:
    const Span span_;
};

class WhileStatement : public Statement {
public:
    WhileStatement(WhileStatementWhileKeyword while_keyword,
                   WhileStatementLParen lparen,
                   const std::shared_ptr<Expression>& cond,
                   WhileStatementRParen rparen,
                   const std::shared_ptr<Statement>& body)
        : while_keyword_(while_keyword),
          lparen_(lparen),
          cond_(cond),
          rparen_(rparen),
          body_(body) {}

    const WhileStatementWhileKeyword& while_keyword() const {
        return while_keyword_;
    }

    const WhileStatementLParen& lparen() const { return lparen_; }

    const std::shared_ptr<Expression>& cond() const { return cond_; }

    const WhileStatementRParen& rparen() const { return rparen_; }

    const std::shared_ptr<Statement>& body() const { return body_; }

    StatementKind kind() const { return StatementKind::While; }

    Span span() const {
        return Span(while_keyword_.span().start(), body_->span().end(),
                    while_keyword_.span().id());
    }

    std::string debug() const {
        auto while_keyword = while_keyword_.debug();
        auto lparen = lparen_.debug();
        auto cond = cond_->debug();
        auto rparen = rparen_.debug();
        auto body = body_->debug();
        return while_keyword + " " + lparen + cond + rparen + " " + body;
    }

private:
    const WhileStatementWhileKeyword while_keyword_;
    const WhileStatementLParen lparen_;
    const std::shared_ptr<Expression> cond_;
    const WhileStatementRParen rparen_;
    const std::shared_ptr<Statement> body_;
};

class DoWhileStatementDoKeyword {
public:
    DoWhileStatementDoKeyword(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "do"; }

private:
    const Span span_;
};

class DoWhileStatementWhileKeyword {
public:
    DoWhileStatementWhileKeyword(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "while"; }

private:
    const Span span_;
};

class DoWhileStatementLParen {
public:
    DoWhileStatementLParen(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "("; }

private:
    const Span span_;
};

class DoWhileStatementRParen {
public:
    DoWhileStatementRParen(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ")"; }

private:
    const Span span_;
};

class DoWhileStatementSemicolon {
public:
    DoWhileStatementSemicolon(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ";"; }

private:
    const Span span_;
};

class DoWhileStatement : public Statement {
public:
    DoWhileStatement(DoWhileStatementDoKeyword do_keyword,
                     const std::shared_ptr<Statement>& body,
                     DoWhileStatementWhileKeyword while_keyword,
                     DoWhileStatementLParen lparen,
                     const std::shared_ptr<Expression>& cond,
                     DoWhileStatementRParen rparen,
                     DoWhileStatementSemicolon semicolon)
        : do_keyword_(do_keyword),
          body_(body),
          while_keyword_(while_keyword),
          lparen_(lparen),
          cond_(cond),
          rparen_(rparen),
          semicolon_(semicolon) {}

    const DoWhileStatementDoKeyword& do_keyword() const { return do_keyword_; }

    const std::shared_ptr<Statement>& body() const { return body_; }

    const DoWhileStatementWhileKeyword& while_keyword() const {
        return while_keyword_;
    }

    const DoWhileStatementLParen& lparen() const { return lparen_; }

    const std::shared_ptr<Expression>& cond() const { return cond_; }

    const DoWhileStatementRParen& rparen() const { return rparen_; }

    const DoWhileStatementSemicolon& semicolon() const { return semicolon_; }

    StatementKind kind() const override { return StatementKind::DoWhile; }

    Span span() const override {
        return Span(do_keyword_.span().start(), semicolon_.span().end(),
                    do_keyword_.span().id());
    }

    std::string debug() const override {
        auto do_keyword = do_keyword_.debug();
        auto body = body_->debug();
        auto while_keyword = while_keyword_.debug();
        auto lparen = lparen_.debug();
        auto cond = cond_->debug();
        auto rparen = rparen_.debug();
        auto semicolon = semicolon_.debug();
        return do_keyword + " " + body + " " + while_keyword + " " + lparen +
               cond + rparen + semicolon;
    }

private:
    const DoWhileStatementDoKeyword do_keyword_;
    const std::shared_ptr<Statement> body_;
    const DoWhileStatementWhileKeyword while_keyword_;
    const DoWhileStatementLParen lparen_;
    const std::shared_ptr<Expression> cond_;
    const DoWhileStatementRParen rparen_;
    const DoWhileStatementSemicolon semicolon_;
};

class ForStatementForKeyword {
public:
    ForStatementForKeyword(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "for"; }

private:
    const Span span_;
};

class ForStatementLParen {
public:
    ForStatementLParen(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "("; }

private:
    const Span span_;
};

class ForStatementRParen {
public:
    ForStatementRParen(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ")"; }

private:
    const Span span_;
};

class ForStatementSemicolon {
public:
    ForStatementSemicolon(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ";"; }

private:
    const Span span_;
};

class ForStatement : public Statement {
public:
    ForStatement(ForStatementForKeyword for_keyword, ForStatementLParen lparen,
                 const std::optional<std::shared_ptr<Expression>>& init,
                 ForStatementSemicolon semicolon1,
                 const std::optional<std::shared_ptr<Expression>>& cond,
                 ForStatementSemicolon semicolon2,
                 const std::optional<std::shared_ptr<Expression>>& update,
                 ForStatementRParen rparen,
                 const std::shared_ptr<Statement>& body)
        : for_keyword_(for_keyword),
          lparen_(lparen),
          init_(init),
          semicolon1_(semicolon1),
          cond_(cond),
          semicolon2_(semicolon2),
          update_(update),
          rparen_(rparen),
          body_(body) {}

    const ForStatementForKeyword& for_keyword() const { return for_keyword_; }

    const ForStatementLParen& lparen() const { return lparen_; }

    const std::optional<std::shared_ptr<Expression>>& init() const {
        return init_;
    }

    const ForStatementSemicolon& semicolon1() const { return semicolon1_; }

    const std::optional<std::shared_ptr<Expression>>& cond() const {
        return cond_;
    }

    const ForStatementSemicolon& semicolon2() const { return semicolon2_; }

    const std::optional<std::shared_ptr<Expression>>& update() const {
        return update_;
    }

    const ForStatementRParen& rparen() const { return rparen_; }

    const std::shared_ptr<Statement>& body() const { return body_; }

    StatementKind kind() const override { return StatementKind::For; }

    Span span() const override {
        return Span(for_keyword_.span().start(), body_->span().end(),
                    for_keyword_.span().id());
    }

    std::string debug() const override {
        auto for_keyword = for_keyword_.debug();
        auto lparen = lparen_.debug();
        auto init = init_.has_value() ? init_.value()->debug() : "";
        auto semicolon1 = semicolon1_.debug();
        auto cond = cond_.has_value() ? cond_.value()->debug() : "";
        auto semicolon2 = semicolon2_.debug();
        auto update = update_.has_value() ? update_.value()->debug() : "";
        auto rparen = rparen_.debug();
        auto body = body_->debug();
        return for_keyword + " " + lparen + init + semicolon1 + " " + cond +
               semicolon2 + " " + update + rparen + " " + body;
    }

private:
    const ForStatementForKeyword for_keyword_;
    const ForStatementLParen lparen_;
    const std::optional<std::shared_ptr<Expression>> init_;
    const ForStatementSemicolon semicolon1_;
    const std::optional<std::shared_ptr<Expression>> cond_;
    const ForStatementSemicolon semicolon2_;
    const std::optional<std::shared_ptr<Expression>> update_;
    const ForStatementRParen rparen_;
    const std::shared_ptr<Statement> body_;
};

class GotoStatementLabel {
public:
    GotoStatementLabel(const std::string& name, Span span)
        : name_(name), span_(span) {}

    const std::string& name() const { return name_; }

    Span span() const { return span_; }

    std::string debug() const { return name_; }

private:
    const std::string name_;
    const Span span_;
};

class GotoStatementGotoKeyword {
public:
    GotoStatementGotoKeyword(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "goto"; }

private:
    const Span span_;
};

class GotoStatementSemicolon {
public:
    GotoStatementSemicolon(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ";"; }

private:
    const Span span_;
};

class GotoStatement : public Statement {
public:
    GotoStatement(GotoStatementGotoKeyword goto_keyword,
                  GotoStatementLabel label, GotoStatementSemicolon semicolon)
        : goto_keyword_(goto_keyword), label_(label), semicolon_(semicolon) {}

    const GotoStatementGotoKeyword& goto_keyword() const {
        return goto_keyword_;
    }

    const GotoStatementLabel& label() const { return label_; }

    const GotoStatementSemicolon& semicolon() const { return semicolon_; }

    StatementKind kind() const override { return StatementKind::Goto; }

    Span span() const override {
        return Span(goto_keyword_.span().start(), semicolon_.span().end(),
                    goto_keyword_.span().id());
    }

    std::string debug() const override {
        auto goto_keyword = goto_keyword_.debug();
        auto label = label_.debug();
        auto semicolon = semicolon_.debug();
        return goto_keyword + " " + label + semicolon;
    }

private:
    const GotoStatementGotoKeyword goto_keyword_;
    const GotoStatementLabel label_;
    const GotoStatementSemicolon semicolon_;
};

class ContinueStatementContinueKeyword {
public:
    ContinueStatementContinueKeyword(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "continue"; }

private:
    const Span span_;
};

class ContinueStatementSemicolon {
public:
    ContinueStatementSemicolon(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ";"; }

private:
    const Span span_;
};

class ContinueStatement : public Statement {
public:
    ContinueStatement(ContinueStatementContinueKeyword continue_keyword,
                      ContinueStatementSemicolon semicolon)
        : continue_keyword_(continue_keyword), semicolon_(semicolon) {}

    const ContinueStatementContinueKeyword& continue_keyword() const {
        return continue_keyword_;
    }

    const ContinueStatementSemicolon& semicolon() const { return semicolon_; }

    StatementKind kind() const override { return StatementKind::Continue; }

    Span span() const override {
        return Span(continue_keyword_.span().start(), semicolon_.span().end(),
                    continue_keyword_.span().id());
    }

    std::string debug() const override {
        auto continue_keyword = continue_keyword_.debug();
        auto semicolon = semicolon_.debug();
        return continue_keyword + semicolon;
    }

private:
    const ContinueStatementContinueKeyword continue_keyword_;
    const ContinueStatementSemicolon semicolon_;
};

class BreakStatementBreakKeyword {
public:
    BreakStatementBreakKeyword(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "break"; }

private:
    const Span span_;
};

class BreakStatementSemicolon {
public:
    BreakStatementSemicolon(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ";"; }

private:
    const Span span_;
};

class BreakStatement : public Statement {
public:
    BreakStatement(BreakStatementBreakKeyword break_keyword,
                   BreakStatementSemicolon semicolon)
        : break_keyword_(break_keyword), semicolon_(semicolon) {}

    const BreakStatementBreakKeyword& break_keyword() const {
        return break_keyword_;
    }

    const BreakStatementSemicolon& semicolon() const { return semicolon_; }

    StatementKind kind() const override { return StatementKind::Break; }

    Span span() const override {
        return Span(break_keyword_.span().start(), semicolon_.span().end(),
                    break_keyword_.span().id());
    }

    std::string debug() const override {
        auto break_keyword = break_keyword_.debug();
        auto semicolon = semicolon_.debug();
        return break_keyword + semicolon;
    }

private:
    const BreakStatementBreakKeyword break_keyword_;
    const BreakStatementSemicolon semicolon_;
};

class ReturnStatementReturnKeyword {
public:
    ReturnStatementReturnKeyword(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return "return"; }

private:
    const Span span_;
};

class ReturnStatementSemicolon {
public:
    ReturnStatementSemicolon(Span span) : span_(span) {}

    Span span() const { return span_; }

    std::string debug() const { return ";"; }

private:
    const Span span_;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(ReturnStatementReturnKeyword return_keyword,
                    const std::shared_ptr<Expression> ret,
                    ReturnStatementSemicolon semicolon)
        : return_keyword_(return_keyword), ret_(ret), semicolon_(semicolon) {}

    const ReturnStatementReturnKeyword& return_keyword() const {
        return return_keyword_;
    }

    const std::shared_ptr<Expression>& ret() const { return ret_; }

    const ReturnStatementSemicolon& semicolon() const { return semicolon_; }

    StatementKind kind() const override { return StatementKind::Return; }

    Span span() const override {
        return Span(return_keyword_.span().start(), semicolon_.span().end(),
                    return_keyword_.span().id());
    }

    std::string debug() const override {
        auto return_keyword = return_keyword_.debug();
        auto ret = ret_->debug();
        auto semicolon = semicolon_.debug();
        return return_keyword + " " + ret + semicolon;
    }

private:
    const ReturnStatementReturnKeyword return_keyword_;
    const std::shared_ptr<Expression> ret_;
    const ReturnStatementSemicolon semicolon_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_STMT_H_
