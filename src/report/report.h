#ifndef TINYC_REPORT_H_
#define TINYC_REPORT_H_

#include "../input/cache.h"
#include "../span.h"

namespace tinyc {

class ErrorReport {
public:
    // Construct a new `ErrorReport` with reference of `InputCache`.
    ErrorReport(InputCache& cache) : cache_(cache) {}

    // Report error to standard error.
    void error(const std::string& msg, Span span);

    // Report warning to standard error.
    void warning(const std::string& msg, Span span);

private:
    InputCache& cache_;
};

}  // namespace tinyc

#endif
