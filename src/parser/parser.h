#ifndef TINYC_PARSER_PARSER_H_
#define TINYC_PARSER_PARSER_H_

#include <exception>
#include <vector>

#include "../lexer/lexer.h"
#include "../report/report.h"

namespace tinyc {

// A class represent an error happen during parsing.
class ParseError : public Reportable {
public:
    // Construct a new `ParseError` with message and its span.
    ParseError(const std::string& what, Span span) : what_(what), span_(span) {}

    ReportLevel level() const override { return ReportLevel::Error; }

    std::string situation() const override { return "while parsing input"; }

    std::string what() const override { return what_; }

    Span span() const override { return span_; }

private:
    const std::string what_;
    const Span span_;
};

class Program {};

Program parse(TokenStream& ts, std::vector<ParseError>& es);

}  // namespace tinyc

#endif  // TINYC_PARSER_PARSER_H_
