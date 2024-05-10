#ifndef TINYC_REPORT_REPORTABLE_H_
#define TINYC_REPORT_REPORTABLE_H_

#include <string>

#include "../span.h"

namespace tinyc {

enum class ReportableLevel {
    Error,
    Warning,
};

// An abstract class for which can be reported via `report`.
class Reportable {
public:
    virtual ~Reportable() {}
    virtual ReportableLevel level() const = 0;
    virtual std::string context() const = 0;
    virtual std::string what() const = 0;
    virtual Span span() const = 0;
};

class SimpleWarning : public Reportable {
public:
    SimpleWarning(const std::string& context, const std::string& what,
                  Span span)
        : context_(context), what_(what), span_(span) {}

    inline ReportableLevel level() const override {
        return ReportableLevel::Warning;
    }

    inline std::string context() const override { return context_; }

    inline std::string what() const override { return what_; }

    inline Span span() const override { return span_; }

private:
    const std::string context_;
    const std::string what_;
    const Span span_;
};

}  // namespace tinyc

#endif  // TINYC_REPORT_REPORTABLE_H_
