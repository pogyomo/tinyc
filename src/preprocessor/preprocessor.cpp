#include "preprocessor.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <sstream>
#include <utility>
#include <vector>

#include "../lexer/lexer.h"
#include "../report/report.h"
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

std::optional<std::vector<std::string>> parse_macro_params(TokenStream& ts,
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
                                                           Span start,
                                                           int row) {
    if (ts.eos() || ts.token()->kind() != TokenKind::Identifier ||
        ts.token()->span().start().row() != row) {
        ts.retrest();
        throw PreProcessError("expected macro name after define",
                              ts.token()->span());
    }
    auto id = std::static_pointer_cast<ValueToken<std::string>>(ts.token());
    auto name = id->value();
    ts.advance();

    auto params = parse_macro_params(ts, row);
    auto body = parse_macro_body(ts, row);
    if (params.has_value()) {
        return {name,
                std::make_shared<FunctionMacro>(name, params.value(), body)};
    } else {
        return {name, std::make_shared<ObjectMacro>(name, body)};
    }
}

std::vector<std::vector<std::shared_ptr<Token>>> parse_macro_args(
    TokenStream& ts, Span start) {
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
            if (unclosing_paren == 1) {
                args.push_back({});
            } else {
                args.back().push_back(ts.token());
            }
            ts.advance();
        } else {
            args.back().push_back(ts.token());
            ts.advance();
        }
    }
}

std::optional<TokenStream> preprocess(Context& ctx, TokenStream&& ts) {
    int waiting_if = 0;
    std::vector<std::shared_ptr<Token>> res;
    while (!ts.eos()) {
        if (ts.token()->kind() == TokenKind::Identifier) {
            auto id =
                std::static_pointer_cast<ValueToken<std::string>>(ts.token());
            if (!ctx.macro_table().has_macro(id->value())) {
                res.push_back(ts.token());
                ts.advance();
                continue;
            }

            auto macro = ctx.macro_table().get_macro(id->value());
            ts.advance();

            try {
                auto args = parse_macro_args(ts, id->span());
                auto expanded = preprocess(ctx, macro->expand(args));
                if (!expanded.has_value()) {
                    return std::nullopt;
                }
                while (!expanded->eos()) {
                    res.push_back(expanded->token());
                    expanded->advance();
                }
                continue;
            } catch (PreProcessError e) {
                report(ctx, e);
                return std::nullopt;
            }
        } else if (ts.token()->kind() != TokenKind::Sharp) {
            res.push_back(ts.token());
            ts.advance();
            continue;
        }
        int row = ts.token()->span().start().row();
        auto start = ts.token()->span();
        ts.advance();

        if (ts.eos() || ts.token()->kind() != TokenKind::Identifier ||
            ts.token()->span().start().row() != row) {
            ts.retrest();
            report(ctx, PreProcessError("expected identifier after #",
                                        ts.token()->span()));
            return std::nullopt;
        }
        auto directive =
            std::static_pointer_cast<ValueToken<std::string>>(ts.token());
        ts.advance();

        if (directive->value() == "define") {
            try {
                auto [name, macro] = parse_macro(ts, start, row);
                ctx.macro_table().add_macro(name, macro);
            } catch (PreProcessError e) {
                report(ctx, e);
                return std::nullopt;
            }
        } else if (directive->value() == "undef") {
            if (ts.eos() || ts.token()->kind() != TokenKind::Identifier ||
                ts.token()->span().start().row() != row) {
                ts.retrest();
                report(ctx, PreProcessError("expected macro name after undef",
                                            ts.token()->span()));
                return std::nullopt;
            }
            auto macro_name =
                std::static_pointer_cast<ValueToken<std::string>>(ts.token())
                    ->value();
            ts.advance();

            ctx.macro_table().del_macro(macro_name);
        } else if (directive->value() == "ifndef") {
            if (ts.eos() || ts.token()->kind() != TokenKind::Identifier ||
                ts.token()->span().start().row() != row) {
                ts.retrest();
                report(ctx, PreProcessError("expected macro name after ifndef",
                                            ts.token()->span()));
                return std::nullopt;
            }
            auto macro_name =
                std::static_pointer_cast<ValueToken<std::string>>(ts.token())
                    ->value();
            Span ifndef_span = concat_spans({start, ts.token()->span()});
            ts.advance();

            if (ctx.macro_table().has_macro(macro_name)) {
                while (true) {
                    if (ts.eos()) {
                        report(ctx, PreProcessError("unclosing ifndef",
                                                    ifndef_span));
                        return std::nullopt;
                    }
                    if (ts.token()->kind() != TokenKind::Sharp) {
                        ts.advance();
                        continue;
                    }
                    int row = ts.token()->span().start().row();
                    ts.advance();

                    if (ts.eos()) {
                        report(ctx, PreProcessError("unclosing ifndef",
                                                    ifndef_span));
                        return std::nullopt;
                    }
                    if (ts.token()->kind() != TokenKind::Identifier ||
                        ts.token()->span().start().row() != row) {
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
        } else if (directive->value() == "ifdef") {
            if (ts.eos() || ts.token()->kind() != TokenKind::Identifier ||
                ts.token()->span().start().row() != row) {
                ts.retrest();
                report(ctx, PreProcessError("expected macro name after ifdef",
                                            ts.token()->span()));
                return std::nullopt;
            }
            auto macro_name =
                std::static_pointer_cast<ValueToken<std::string>>(ts.token())
                    ->value();
            Span ifdef_span = concat_spans({start, ts.token()->span()});
            ts.advance();

            if (!ctx.macro_table().has_macro(macro_name)) {
                while (true) {
                    if (ts.eos()) {
                        report(ctx,
                               PreProcessError("unclosing ifdef", ifdef_span));
                        return std::nullopt;
                    }
                    if (ts.token()->kind() != TokenKind::Sharp) {
                        ts.advance();
                        continue;
                    }
                    int row = ts.token()->span().start().row();
                    ts.advance();

                    if (ts.eos()) {
                        report(ctx,
                               PreProcessError("unclosing ifdef", ifdef_span));
                        return std::nullopt;
                    }
                    if (ts.token()->kind() != TokenKind::Identifier ||
                        ts.token()->span().start().row() != row) {
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
        } else if (directive->value() == "endif") {
            if (waiting_if == 0) {
                Span span = concat_spans({start, directive->span()});
                report(ctx, PreProcessError("extra endif", span));
                return std::nullopt;
            }
            waiting_if--;
        } else if (directive->value() == "include") {
            if (ts.eos() || ts.token()->kind() != TokenKind::String ||
                ts.token()->span().start().row() != row) {
                ts.retrest();
                report(ctx,
                       PreProcessError("expected path string after include",
                                       ts.token()->span()));
                return std::nullopt;
            }
            auto path_s =
                std::static_pointer_cast<ValueToken<std::string>>(ts.token())
                    ->value();
            std::filesystem::path path(path_s);
            ts.advance();

            std::fstream fs(path);
            auto included = lex(ctx, fs, path.filename());
            if (!included.has_value()) {
                return std::nullopt;
            }
            while (!included->eos()) {
                res.push_back(included->token());
                included->advance();
            }
        } else if (directive->value() == "error") {
            Span span = directive->span();

            auto body = parse_macro_body(ts, row);
            std::stringstream what;
            if (!body.empty()) {
                what << body[0]->debug();
                for (int i = 1; i < body.size(); i++)
                    what << " " << body[i]->debug();
            }

            report(ctx, PreProcessError(what.str(), span));
            return std::nullopt;
        } else {
            std::stringstream ss;
            ss << "unknown directive name: " << directive->value();
            report(ctx, PreProcessError(ss.str(), directive->span()));
            return std::nullopt;
        }
    }
    return res;
}

}  // namespace tinyc
