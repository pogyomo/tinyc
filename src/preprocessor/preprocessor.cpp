#include "preprocessor.h"

#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "error.h"
#include "macro.h"

namespace tinyc {

std::vector<std::shared_ptr<Token>> parse_macro_body(TokenStream& ts, int row) {
    std::vector<std::shared_ptr<Token>> body;
    while (!ts.eos() && ts.token()->span().start().row() == row) {
        body.push_back(ts.token());
        ts.advance();
    }
    return body;
}

std::optional<std::vector<std::string>> parse_fun_params(TokenStream& ts,
                                                         int row) {
    auto state = ts.get_state();

    if (ts.eos() || ts.token()->kind() != TokenKind::LParen) {
        ts.set_state(state);
        return std::nullopt;
    }
    ts.advance();

    std::vector<std::string> params;
    while (true) {
        if (ts.eos() || ts.token()->span().start().row() != row) {
            ts.set_state(state);
            return std::nullopt;
        } else if (ts.token()->kind() == TokenKind::RParen) {
            ts.advance();
            return params;
        }

        if (ts.eos() || ts.token()->kind() != TokenKind::Identifier) {
            ts.set_state(state);
            return std::nullopt;
        }
        auto id = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        params.push_back(id->value());
        ts.advance();

        if (ts.eos() || ts.token()->span().start().row() != row) {
            ts.set_state(state);
            return std::nullopt;
        } else if (ts.token()->kind() == TokenKind::RParen) {
            continue;
        } else if (ts.token()->kind() == TokenKind::Comma) {
            ts.advance();
        } else {
            ts.set_state(state);
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
    int row = id->span().start().row();
    ts.advance();

    auto params = parse_fun_params(ts, row);
    auto body = parse_macro_body(ts, row);
    if (params.has_value()) {
        return {name,
                std::make_shared<FunctionMacro>(name, params.value(), body)};
    } else {
        return {name, std::make_shared<ObjectMacro>(name, body)};
    }
}

std::vector<std::vector<std::shared_ptr<Token>>> parse_fun_args(TokenStream& ts,
                                                                Span start) {
    if (ts.eos() || ts.token()->kind() != TokenKind::LParen) return {};
    ts.advance();

    int unclosing_paren = 1;
    std::vector<std::vector<std::shared_ptr<Token>>> args;
    args.push_back({});
    while (true) {
        if (ts.eos()) {
            ts.retrest();
            throw PreProcessError("unclosing function macro",
                                  concat_spans({start, ts.token()->span()}));
        } else if (ts.token()->kind() == TokenKind::LParen) {
            unclosing_paren++;
            args.back().push_back(ts.token());
            ts.advance();
        } else if (ts.token()->kind() == TokenKind::RParen) {
            if (unclosing_paren == 1) {
                ts.advance();
                return args;
            } else {
                unclosing_paren--;
                args.back().push_back(ts.token());
                ts.advance();
            }
        } else if (ts.token()->kind() == TokenKind::Comma) {
            args.push_back({});
            ts.advance();
        } else {
            args.back().push_back(ts.token());
            ts.advance();
        }
    }
}

TokenStream preprocess(Context& ctx, TokenStream&& ts) {
    int waiting_if = 0;
    std::vector<std::shared_ptr<Token>> res;
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Identifier) {
            auto id =
                std::static_pointer_cast<ValueToken<std::string>>(ts.token());
            try {
                auto macro = ctx.macro_table().get_macro(id->value());
                ts.advance();
                auto args = parse_fun_args(ts, id->span());
                auto expanded = macro->expand(args);
                res.insert(res.end(), expanded.begin(), expanded.end());
                continue;
            } catch (std::out_of_range e) {
                res.push_back(ts.token());
                ts.advance();
                continue;
            }
        } else if (ts.token()->kind() != TokenKind::Sharp) {
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
        auto directive_name =
            std::static_pointer_cast<ValueToken<std::string>>(ts.token())
                ->value();
        if (directive_name == "define") {
            ts.advance();
            auto [name, macro] = parse_macro(ts, start);
            ctx.macro_table().add_macro(name, macro);
        } else if (directive_name == "ifndef") {
            ts.advance();
            if (ts.eos() || ts.token()->kind() != TokenKind::Identifier) {
                ts.retrest();
                throw PreProcessError("expected macro name after ifndef",
                                      ts.token()->span());
            }
            auto macro_name =
                std::static_pointer_cast<ValueToken<std::string>>(ts.token())
                    ->value();
            Span ifndef_span = concat_spans({start, ts.token()->span()});
            ts.advance();
            if (ctx.macro_table().has_macro(macro_name)) {
                while (true) {
                    if (ts.eos()) {
                        throw PreProcessError("unclosing ifndef", ifndef_span);
                    }
                    if (ts.token()->kind() != TokenKind::Sharp) {
                        ts.advance();
                        continue;
                    }
                    ts.advance();

                    if (ts.eos()) {
                        throw PreProcessError("unclosing ifndef", ifndef_span);
                    }
                    if (ts.token()->kind() != TokenKind::Identifier) {
                        ts.advance();
                        continue;
                    }
                    auto endif_name =
                        std::static_pointer_cast<ValueToken<std::string>>(
                            ts.token())
                            ->value();
                    ts.advance();

                    if (endif_name == "endif") {
                        break;
                    }
                }
            } else {
                waiting_if++;
            }
        } else if (directive_name == "ifdef") {
            ts.advance();
            if (ts.eos() || ts.token()->kind() != TokenKind::Identifier) {
                ts.retrest();
                throw PreProcessError("expected macro name after ifdef",
                                      ts.token()->span());
            }
            auto macro_name =
                std::static_pointer_cast<ValueToken<std::string>>(ts.token())
                    ->value();
            Span ifdef_span = concat_spans({start, ts.token()->span()});
            ts.advance();
            if (!ctx.macro_table().has_macro(macro_name)) {
                while (true) {
                    if (ts.eos()) {
                        throw PreProcessError("unclosing ifdef", ifdef_span);
                    }
                    if (ts.token()->kind() != TokenKind::Sharp) {
                        ts.advance();
                        continue;
                    }
                    ts.advance();

                    if (ts.eos()) {
                        throw PreProcessError("unclosing ifdef", ifdef_span);
                    }
                    if (ts.token()->kind() != TokenKind::Identifier) {
                        ts.advance();
                        continue;
                    }
                    auto endif_name =
                        std::static_pointer_cast<ValueToken<std::string>>(
                            res.back())
                            ->value();
                    ts.advance();

                    if (endif_name == "endif") {
                        break;
                    }
                }
            } else {
                waiting_if++;
            }
        } else if (directive_name == "endif") {
            if (waiting_if == 0) {
                Span span = concat_spans({start, ts.token()->span()});
                throw PreProcessError("extra endif", span);
            }
            ts.advance();
            waiting_if--;
        } else {
            std::stringstream ss;
            ss << "unknown directive name: " << directive_name;
            throw PreProcessError(ss.str(), ts.token()->span());
        }
    }
    return res;
}

}  // namespace tinyc
