#ifndef TINYC_REPORT_H_
#define TINYC_REPORT_H_

#include "../input/cache.h"
#include "../span.h"

namespace tinyc {

enum class ReportLevel {
    Error,
    Warning,
};

// An abstract class for which can report via `Reporter`.
class Reportable {
public:
    virtual ~Reportable() {}
    virtual ReportLevel level() const = 0;
    virtual std::string situation() const = 0;
    virtual std::string what() const = 0;
    virtual Span span() const = 0;
};

// A class which report arbitrary error, warning or etc.
class Reporter {
public:
    // Construct a new `Reporter` with reference of `InputCache`.
    Reporter(InputCache& cache) : cache_(cache) {}

    // Report given `Reportable`.
    void report(const Reportable& r);

private:
    InputCache& cache_;
};

}  // namespace tinyc

#endif
