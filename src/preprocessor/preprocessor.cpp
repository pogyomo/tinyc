#include "preprocessor.h"

#include <memory>
#include <optional>
#include <sstream>
#include <utility>
#include <vector>

#include "error.h"
#include "macro.h"

namespace tinyc {

std::vector<std::shared_ptr<Token>> parse_macro_body(TokenStream& ts) {
    auto row = ts.token()->span().start().row();
    std::vector<std::shared_ptr<Token>> body;
    while (!ts.eos() && ts.token()->span().start().row() == row) {
        body.push_back(ts.token());
        ts.advance();
    }
    return body;
}

std::optional<std::vector<std::string>> parse_fun_params(TokenStream& ts) {
    ts.push_state();

    if (ts.eos() || ts.token()->kind() != TokenKind::LParen) {
        ts.pop_state();
        return std::nullopt;
    }
    ts.advance();

    std::vector<std::string> params;
    while (true) {
        if (ts.eos()) {
            ts.pop_state();
            return std::nullopt;
        } else if (ts.token()->kind() == TokenKind::RParen) {
            ts.advance();
            return params;
        }

        if (ts.token()->kind() != TokenKind::Identifier) {
            ts.pop_state();
            return std::nullopt;
        }
        auto id = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        params.push_back(id->value());
        ts.advance();

        if (ts.eos()) {
            ts.pop_state();
            return std::nullopt;
        } else if (ts.token()->kind() == TokenKind::RParen) {
            continue;
        } else if (ts.token()->kind() == TokenKind::Comma) {
            ts.advance();
        } else {
            ts.pop_state();
            return std::nullopt;
        }
    }
}

std::pair<std::string, std::shared_ptr<Macro>> parse_macro(TokenStream& ts,
                                                           Span start) {
    if (ts.eos() || ts.token()->kind() != TokenKind::Identifier) {
        ts.retrest();
        throw PreProcessError("expected macro name after define",
                              ts.token()->span());
    }
    auto id = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
    auto name = id->value();
    ts.advance();

    auto params = parse_fun_params(ts);
    auto body = parse_macro_body(ts);
    if (params.has_value()) {
        return {name,
                std::make_shared<FunctionMacro>(name, params.value(), body)};
    } else {
        return {name, std::make_shared<ObjectMacro>(name, body)};
    }
}

TokenStream preprocess(Context& ctx, TokenStream&& ts) {
    std::vector<std::shared_ptr<Token>> res;
    while (!ts.eos()) {
        if (ts.token()->kind() != TokenKind::Sharp) {
            res.push_back(ts.token());
            ts.advance();
            continue;
        }
        auto start = ts.token()->span();
        ts.advance();

        if (ts.eos() || ts.token()->kind() != TokenKind::Identifier) {
            ts.retrest();
            throw PreProcessError("expected identifier after #",
                                  ts.token()->span());
        }
        auto id = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        if (id->value() == "define") {
            ts.advance();
            auto [name, macro] = parse_macro(ts, start);
            ctx.macro_table().add_macro(name, macro);
        } else {
            std::stringstream ss;
            ss << "unknown directive name: " << id->value();
            throw PreProcessError(ss.str(), id->span());
        }
    }
    return res;
}

}  // namespace tinyc
