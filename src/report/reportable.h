#ifndef TINYC_REPORT_REPORTABLE_H_
#define TINYC_REPORT_REPORTABLE_H_

#include <optional>
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
    virtual std::string what() const = 0;
    virtual std::optional<std::string> additional() const = 0;
    virtual Span span() const = 0;
};

class SimpleReportable : public Reportable {
public:
    SimpleReportable(ReportableLevel level, const std::string& what,
                     const std::optional<std::string>& additional, Span span)
        : level_(level), what_(what), additional_(additional), span_(span) {}

    inline ReportableLevel level() const override { return level_; }

    inline std::string what() const override { return what_; }

    inline std::optional<std::string> additional() const override {
        return additional_;
    }

    inline Span span() const override { return span_; }

private:
    const ReportableLevel level_;
    const std::string what_;
    const std::optional<std::string> additional_;
    const Span span_;
};

class SimpleError : public SimpleReportable {
public:
    SimpleError(const std::string& what,
                const std::optional<std::string>& additional, Span span)
        : SimpleReportable(ReportableLevel::Error, what, additional, span) {}
};

class SimpleWarning : public SimpleReportable {
public:
    SimpleWarning(const std::string& what,
                  const std::optional<std::string>& additional, Span span)
        : SimpleReportable(ReportableLevel::Warning, what, additional, span) {}
};

}  // namespace tinyc

#endif  // TINYC_REPORT_REPORTABLE_H_
