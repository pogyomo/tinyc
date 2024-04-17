#ifndef TINYC_PARSER_H_
#define TINYC_PARSER_H_

#include <exception>
#include <vector>

#include "lexer.h"

namespace tinyc {

class ParseError : public std::exception {
public:
    ParseError(const std::string& msg, Span span) : msg_(msg), span_(span) {}

    const std::string& msg() const { return msg_; }

    const Span& span() const { return span_; }

private:
    const std::string msg_;
    const Span span_;
};

class Program {};

Program parse(TokenStream& ts, std::vector<ParseError>& es);

}  // namespace tinyc

#endif
