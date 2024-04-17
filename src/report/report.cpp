#include "report.h"

#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "../input/input.h"
#include "color.h"

namespace tinyc {

enum class ReportLevel {
    Error,
    Warning,
};

// Generate colored string such that "error: ", "warning:", etc. and its length
// when it is displayed.
std::pair<std::string, int> gen_info(const std::string& name,
                                     std::pair<int, int> start,
                                     ReportLevel level, bool show_level) {
    std::string s = "";
    if (level == ReportLevel::Error) {
        s = "error: ";
    } else {
        s = "warning: ";
    }
    ColoredString slevel(s);
    if (level == ReportLevel::Error) {
        slevel.set_fg_red();
    } else {
        slevel.set_fg_yellow();
    }

    std::stringstream info;
    info << name << ":" << start.first << ":" << start.second << ": ";
    int len = info.str().size() + slevel.size();
    if (show_level) {
        info << slevel.str();
    } else {
        info << std::string(slevel.size(), ' ');
    }
    return {info.str(), len};
}

void report(InputCache& cache, const std::string& msg, Span span,
            ReportLevel level) {
    auto name = cache.fetch_name(span.id());
    auto input = cache.fetch_input(span.id());
    if (span.start_row() == span.end_row()) {
        auto [info, info_len] = gen_info(name, span.start(), level, true);

        auto line = input.line(span.start_row());
        std::cerr << info << line << std::endl;
        std::cerr << std::string(span.start_offset() + info_len, ' ');
        std::cerr << std::string(span.end_offset() - span.start_offset() + 1,
                                 '^');
        std::cerr << "-- " << msg << std::endl;
    } else {
        auto [sinfo, sinfo_len] = gen_info(name, span.start(), level, true);
        auto [einfo, einfo_len] = gen_info(name, span.end(), level, false);
        auto info_len = sinfo_len > einfo_len ? sinfo_len : einfo_len;

        auto sline = input.line(span.start_row());
        std::cerr << sinfo << sline << std::endl;
        std::cerr << std::string(span.start_offset() + info_len, ' ');
        std::cerr << std::string(sline.size() - span.start_offset(), '^');
        std::cerr << std::endl;

        auto eline = input.line(span.end_row());
        std::cerr << einfo << eline << std::endl;
        std::cerr << std::string(info_len, ' ');
        std::cerr << std::string(span.end_offset() + 1, '^');
        std::cerr << "-- " << msg << std::endl;
    }
}

void ErrorReport::error(const std::string& msg, Span span) {
    report(cache_, msg, span, ReportLevel::Error);
}

void ErrorReport::warning(const std::string& msg, Span span) {
    report(cache_, msg, span, ReportLevel::Warning);
}

}  // namespace tinyc
