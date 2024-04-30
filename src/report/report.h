#ifndef TINYC_REPORT_REPORT_H_
#define TINYC_REPORT_REPORT_H_

#include "../context.h"
#include "reportable.h"

namespace tinyc {

// Report the `Reportable` to stderr.
void report(Context& ctx, Reportable& r);

}  // namespace tinyc

#endif  // TINYC_REPORT_REPORT_H_
