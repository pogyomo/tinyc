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

}  // namespace tinyc

#endif  // TINYC_REPORT_REPORTABLE_H_
