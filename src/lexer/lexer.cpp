#include "lexer.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "../input/stream.h"
#include "../preprocessor/preprocessor.h"
#include "../report/report.h"
#include "../span.h"
#include "stream.h"
#include "token.h"

namespace tinyc {

// Advance `is` until it reach to non-whitespace character, or advance to end
// if `is` has only whitespaces.
// Returns true if it consume whitespaces.
bool skip_whitespaces(InputStream& is) {
    bool skipped = false;
    while (!is.eos()) {
        if (std::isspace(is.ch())) {
            skipped = true;
            is.advance();
        } else {
            break;
        }
    }
    return skipped;
}

// Advance `is` to next line or eos if `is` start with `//`.
// Returns true if it consume comment.
bool skip_oneline_comment(InputStream& is) {
    if (is.eos()) {
        return false;
    }

    int lrow = is.lrow();
    if (is.accept("//")) {
        while (!is.eos() && is.lrow() == lrow) {
            is.advance();
        }
        return true;
    } else {
        return false;
    }
}

// Advance `is` to consume the characters between `/*` and `*/`.
// Returns true if it consume comment.
bool skip_multiline_comment(InputStream& is) {
    if (is.accept("/*")) {
        while (!is.accept("*/")) {
            is.advance();
        }
        return true;
    } else {
        return false;
    }
}

// Skip comments and whitespaces.
void skips(InputStream& is) {
    while (true) {
        if (skip_whitespaces(is)) {
            continue;
        }
        if (skip_oneline_comment(is)) {
            continue;
        }
        if (skip_multiline_comment(is)) {
            continue;
        }
        break;
    }
}

bool is_ident_head(char c) { return std::isalpha(c) || c == '_'; }

bool is_ident_rest(char c) { return std::isalnum(c) || c == '_'; }

std::optional<char> consume_char(Context& ctx, InputStream& is) {
    char ch = is.ch();
    if (ch == '\\') {
        Span span(is.input().id(), is.pos(), is.pos());
        is.advance();

        if (is.eos()) {
            report(ctx,
                   SimpleError("no character after \\", std::nullopt, span));
            return std::nullopt;
        }

        ch = is.ch();
        span = Span(is.input().id(), is.pos(), is.pos());
        is.advance();
        if (ch == 'a') {
            return 0x07;
        } else if (ch == 'b') {
            return 0x08;
        } else if (ch == 'e') {
            return 0x1B;
        } else if (ch == 'f') {
            return 0x0C;
        } else if (ch == 'n') {
            return 0x0A;
        } else if (ch == 'r') {
            return 0x0D;
        } else if (ch == 't') {
            return 0x09;
        } else if (ch == 'v') {
            return 0x0B;
        } else if (ch == '\\') {
            return 0x5C;
        } else if (ch == '\'') {
            return 0x27;
        } else if (ch == '"') {
            return 0x22;
        } else if (ch == '?') {
            return 0x3F;
        } else if (ch == '0') {
            return 0x00;
        } else {
            report(ctx,
                   SimpleError("unknown escape sequence", std::nullopt, span));
            return std::nullopt;
        }
    } else {
        is.advance();
        return ch;
    }
}

// Extract a toke from non-empty `is`.
// If unknown character found, skip the character, reportt error, then return
// std::nullopt.
std::optional<std::shared_ptr<Token>> token(Context& ctx, InputStream& is) {
    int lrow = is.lrow();
    Span span = Span(is.input().id(), is.pos(), is.pos());
    if (is.accept("&&", span)) {
        return std::make_shared<SymbolToken>(TokenKind::And, span, lrow);
    } else if (is.accept("&=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::AndAssign, span, lrow);
    } else if (is.accept('&', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Ampersand, span, lrow);
    } else if (is.accept("||", span)) {
        return std::make_shared<SymbolToken>(TokenKind::Or, span, lrow);
    } else if (is.accept("|=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::OrAssign, span, lrow);
    } else if (is.accept('|', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Vertical, span, lrow);
    } else if (is.accept("^=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::XorAssign, span, lrow);
    } else if (is.accept('^', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Hat, span, lrow);
    } else if (is.accept("==", span)) {
        return std::make_shared<SymbolToken>(TokenKind::EQ, span, lrow);
    } else if (is.accept('=', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Assign, span, lrow);
    } else if (is.accept("!=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::NE, span, lrow);
    } else if (is.accept('!', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Not, span, lrow);
    } else if (is.accept("<<=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::LShiftAssign, span,
                                             lrow);
    } else if (is.accept("<<", span)) {
        return std::make_shared<SymbolToken>(TokenKind::LShift, span, lrow);
    } else if (is.accept("<=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::LE, span, lrow);
    } else if (is.accept('<', span)) {
        return std::make_shared<SymbolToken>(TokenKind::LT, span, lrow);
    } else if (is.accept(">>=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::RShiftAssign, span,
                                             lrow);
    } else if (is.accept(">>", span)) {
        return std::make_shared<SymbolToken>(TokenKind::RShift, span, lrow);
    } else if (is.accept(">=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::GE, span, lrow);
    } else if (is.accept('>', span)) {
        return std::make_shared<SymbolToken>(TokenKind::GT, span, lrow);
    } else if (is.accept("++", span)) {
        return std::make_shared<SymbolToken>(TokenKind::PlusPlus, span, lrow);
    } else if (is.accept("+=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::AddAssign, span, lrow);
    } else if (is.accept('+', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Plus, span, lrow);
    } else if (is.accept("--", span)) {
        return std::make_shared<SymbolToken>(TokenKind::MinusMinus, span, lrow);
    } else if (is.accept("-=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::SubAssign, span, lrow);
    } else if (is.accept("->", span)) {
        return std::make_shared<SymbolToken>(TokenKind::Arrow, span, lrow);
    } else if (is.accept('-', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Minus, span, lrow);
    } else if (is.accept("*=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::MulAssign, span, lrow);
    } else if (is.accept('*', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Star, span, lrow);
    } else if (is.accept("/=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::DivAssign, span, lrow);
    } else if (is.accept('/', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Slash, span, lrow);
    } else if (is.accept("%=", span)) {
        return std::make_shared<SymbolToken>(TokenKind::ModAssign, span, lrow);
    } else if (is.accept('%', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Percent, span, lrow);
    } else if (is.accept('(', span)) {
        return std::make_shared<SymbolToken>(TokenKind::LParen, span, lrow);
    } else if (is.accept(')', span)) {
        return std::make_shared<SymbolToken>(TokenKind::RParen, span, lrow);
    } else if (is.accept('{', span)) {
        return std::make_shared<SymbolToken>(TokenKind::LCurly, span, lrow);
    } else if (is.accept('}', span)) {
        return std::make_shared<SymbolToken>(TokenKind::RCurly, span, lrow);
    } else if (is.accept('[', span)) {
        return std::make_shared<SymbolToken>(TokenKind::LSquare, span, lrow);
    } else if (is.accept(']', span)) {
        return std::make_shared<SymbolToken>(TokenKind::RSquare, span, lrow);
    } else if (is.accept(';', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Semicolon, span, lrow);
    } else if (is.accept(':', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Colon, span, lrow);
    } else if (is.accept(',', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Comma, span, lrow);
    } else if (is.accept('~', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Tilde, span, lrow);
    } else if (is.accept('?', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Question, span, lrow);
    } else if (is.accept('#', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Sharp, span, lrow);
    } else if (is.accept('.', span)) {
        return std::make_shared<SymbolToken>(TokenKind::Dot, span, lrow);
    } else if (is.accept('\'', span)) {
        int lrow = is.lrow();

        auto ch = consume_char(ctx, is);
        if (!ch) {
            return std::nullopt;
        }

        span = Span(is.input().id(), span.start(), is.pos());
        if (is.eos() || is.ch() != '\'') {
            report(ctx, SimpleError("unclosing character literal", std::nullopt,
                                    span));
            return std::nullopt;
        } else {
            is.advance();
            return std::make_shared<ValueToken<char>>(TokenKind::Character,
                                                      ch.value(), span, lrow);
        }
    } else if (is.accept('"', span)) {
        int lrow = is.lrow();

        std::string s;
        Position end = is.pos();
        while (true) {
            if (is.eos() || is.lrow() != lrow) {
                report(ctx, SimpleError("unclosing string literal",
                                        std::nullopt, span));
                return std::nullopt;
            }
            end = is.pos();
            if (is.ch() == '"') {
                is.advance();
                break;
            } else {
                auto ch = consume_char(ctx, is);
                if (!ch) {
                    return std::nullopt;
                }
                s.push_back(ch.value());
            }
        }
        return std::make_shared<ValueToken<std::string>>(
            TokenKind::String, s, Span(is.input().id(), span.start(), end),
            lrow);
    } else if (is.accept("0x", span)) {
        int lrow = is.lrow();

        std::string buf;
        Position end = is.pos();
        while (!is.eos() && is.lrow() == lrow) {
            if ('0' <= is.ch() && is.ch() <= '9' ||
                'a' <= is.ch() && is.ch() <= 'f' ||
                'A' <= is.ch() && is.ch() <= 'F') {
                end = is.pos();
                buf.push_back(is.ch());
                is.advance();
            } else {
                break;
            }
        }

        span = Span(span.id(), span.start(), end);
        if (buf.empty()) {
            report(ctx,
                   SimpleError("empty integer literal", std::nullopt, span));
            return std::nullopt;
        }
        long long value = std::stoull(buf, nullptr, 16);
        return std::make_shared<ValueToken<unsigned long long>>(
            TokenKind::Integer, value, span, lrow);
    } else if (is.accept('0', span)) {
        int lrow = is.lrow();

        std::string buf;
        Position end = is.pos();
        while (!is.eos() && is.lrow() == lrow) {
            if ('0' <= is.ch() && is.ch() <= '7') {
                end = is.pos();
                buf.push_back(is.ch());
                is.advance();
            } else {
                break;
            }
        }

        if (buf.empty()) {
            return std::make_shared<ValueToken<unsigned long long>>(
                TokenKind::Integer, 0, span, lrow);
        } else {
            long long value = std::stoull(buf, nullptr, 8);
            return std::make_shared<ValueToken<unsigned long long>>(
                TokenKind::Integer, value,
                Span(is.input().id(), span.start(), end), lrow);
        }
    } else if (std::isdigit(is.ch())) {
        int lrow = is.lrow();

        std::string buf;
        Position end = is.pos();
        while (!is.eos() && is.lrow() == lrow) {
            if (std::isdigit(is.ch())) {
                end = is.pos();
                buf.push_back(is.ch());
                is.advance();
            } else {
                break;
            }
        }

        span = Span(span.id(), span.start(), end);
        if (buf.empty()) {
            report(ctx,
                   SimpleError("empty integer literal", std::nullopt, span));
            return std::nullopt;
        }
        auto value = std::stoull(buf);
        return std::make_shared<ValueToken<unsigned long long>>(
            TokenKind::Integer, value, span, lrow);
    } else if (is_ident_head(is.ch())) {
        int lrow = is.lrow();

        std::string buf;
        Position start = is.pos();
        Position end = is.pos();
        while (!is.eos() && is.lrow() == lrow) {
            if (is_ident_rest(is.ch())) {
                end = is.pos();
                buf.push_back(is.ch());
                is.advance();
            } else {
                break;
            }
        }

        span = Span(is.input().id(), span.start(), end);
        if (buf == "auto") {
            return std::make_shared<KeywordToken>(TokenKind::Auto, span, lrow);
        } else if (buf == "break") {
            return std::make_shared<KeywordToken>(TokenKind::Break, span, lrow);
        } else if (buf == "case") {
            return std::make_shared<KeywordToken>(TokenKind::Case, span, lrow);
        } else if (buf == "char") {
            return std::make_shared<KeywordToken>(TokenKind::Char, span, lrow);
        } else if (buf == "const") {
            return std::make_shared<KeywordToken>(TokenKind::Const, span, lrow);
        } else if (buf == "continue") {
            return std::make_shared<KeywordToken>(TokenKind::Continue, span,
                                                  lrow);
        } else if (buf == "default") {
            return std::make_shared<KeywordToken>(TokenKind::Default, span,
                                                  lrow);
        } else if (buf == "do") {
            return std::make_shared<KeywordToken>(TokenKind::Do, span, lrow);
        } else if (buf == "double") {
            return std::make_shared<KeywordToken>(TokenKind::Double, span,
                                                  lrow);
        } else if (buf == "else") {
            return std::make_shared<KeywordToken>(TokenKind::Else, span, lrow);
        } else if (buf == "enum") {
            return std::make_shared<KeywordToken>(TokenKind::Enum, span, lrow);
        } else if (buf == "extern") {
            return std::make_shared<KeywordToken>(TokenKind::Extern, span,
                                                  lrow);
        } else if (buf == "float") {
            return std::make_shared<KeywordToken>(TokenKind::Float, span, lrow);
        } else if (buf == "for") {
            return std::make_shared<KeywordToken>(TokenKind::For, span, lrow);
        } else if (buf == "goto") {
            return std::make_shared<KeywordToken>(TokenKind::Goto, span, lrow);
        } else if (buf == "if") {
            return std::make_shared<KeywordToken>(TokenKind::If, span, lrow);
        } else if (buf == "inline") {
            return std::make_shared<KeywordToken>(TokenKind::Inline, span,
                                                  lrow);
        } else if (buf == "int") {
            return std::make_shared<KeywordToken>(TokenKind::Int, span, lrow);
        } else if (buf == "long") {
            return std::make_shared<KeywordToken>(TokenKind::Long, span, lrow);
        } else if (buf == "register") {
            return std::make_shared<KeywordToken>(TokenKind::Register, span,
                                                  lrow);
        } else if (buf == "restrict") {
            return std::make_shared<KeywordToken>(TokenKind::Restrict, span,
                                                  lrow);
        } else if (buf == "return") {
            return std::make_shared<KeywordToken>(TokenKind::Return, span,
                                                  lrow);
        } else if (buf == "short") {
            return std::make_shared<KeywordToken>(TokenKind::Short, span, lrow);
        } else if (buf == "signed") {
            return std::make_shared<KeywordToken>(TokenKind::Signed, span,
                                                  lrow);
        } else if (buf == "sizeof") {
            return std::make_shared<KeywordToken>(TokenKind::Sizeof, span,
                                                  lrow);
        } else if (buf == "static") {
            return std::make_shared<KeywordToken>(TokenKind::Static, span,
                                                  lrow);
        } else if (buf == "struct") {
            return std::make_shared<KeywordToken>(TokenKind::Struct, span,
                                                  lrow);
        } else if (buf == "switch") {
            return std::make_shared<KeywordToken>(TokenKind::Switch, span,
                                                  lrow);
        } else if (buf == "typedef") {
            return std::make_shared<KeywordToken>(TokenKind::Typedef, span,
                                                  lrow);
        } else if (buf == "union") {
            return std::make_shared<KeywordToken>(TokenKind::Union, span, lrow);
        } else if (buf == "unsigned") {
            return std::make_shared<KeywordToken>(TokenKind::Unsigned, span,
                                                  lrow);
        } else if (buf == "void") {
            return std::make_shared<KeywordToken>(TokenKind::Void, span, lrow);
        } else if (buf == "volatile") {
            return std::make_shared<KeywordToken>(TokenKind::Volatile, span,
                                                  lrow);
        } else if (buf == "while") {
            return std::make_shared<KeywordToken>(TokenKind::While, span, lrow);
        } else {
            return std::make_shared<ValueToken<std::string>>(
                TokenKind::Identifier, buf, span, lrow);
        }
    } else {
        std::stringstream ss;
        ss << "unexpected character '" << is.ch() << "' found";
        report(ctx, SimpleError(ss.str(), std::nullopt, span));
        return std::nullopt;
    }
}

std::optional<TokenStream> lex(Context& ctx, std::istream& is,
                               const std::string& name) {
    int id = ctx.input_cache().cache(is, name);
    auto input = ctx.input_cache().fetch(id);
    auto is_ = InputStream(input);

    bool has_error = false;
    std::vector<std::shared_ptr<Token>> ts;
    skips(is_);
    while (!is_.eos()) {
        auto tk = token(ctx, is_);
        if (tk) {
            ts.push_back(tk.value());
        }
        skips(is_);
    }

    if (has_error) {
        return std::nullopt;
    } else {
        return preprocess(ctx, ts);
    }
}

}  // namespace tinyc
