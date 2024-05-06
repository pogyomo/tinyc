#ifndef TINYC_PARSER_ERROR_H_
#define TINYC_PARSER_ERROR_H_

#include "../report/reportable.h"

namespace tinyc {

class ParseError : public Reportable {
public:
    ParseError(const std::string& what, Span span) : what_(what), span_(span) {}

    inline ReportableLevel level() const override {
        return ReportableLevel::Error;
    }

    inline std::string context() const override { return "while lexing input"; }

    inline std::string what() const override { return what_; }

    inline Span span() const override { return span_; }

private:
    const std::string what_;
    const Span span_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_ERROR_H_
