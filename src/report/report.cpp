#include "report.h"

#include <iostream>

#include "color.h"
#include "reportable.h"

namespace tinyc {

// Generate colored string such that "error: ", "warning:", etc. with location
// and its length when it is displayed.
std::pair<std::string, int> gen_info(const std::string& name, Position start,
                                     ReportableLevel level, bool show_level) {
    std::string s = "";
    if (level == ReportableLevel::Error) {
        s = "error: ";
    } else {
        s = "warning: ";
    }
    ColoredString slevel(s);
    if (level == ReportableLevel::Error) {
        slevel.set_fg_red();
    } else {
        slevel.set_fg_yellow();
    }

    std::stringstream info;
    info << name << ":" << start.row() << ":" << start.offset() << ": ";
    int len = info.str().size() + slevel.size();
    if (show_level) {
        info << slevel.str();
    } else {
        info << std::string(slevel.size(), ' ');
    }
    return {info.str(), len};
}

void report(Context& ctx, const Reportable& r) {
    auto span = r.span();
    auto level = r.level();
    auto input = ctx.input_cache().fetch(span.id());
    auto name = input.name();
    if (span.start().row() == span.end().row()) {
        auto [info, info_len] = gen_info(name, span.start(), level, true);

        std::cerr << name << ": " << r.context() << std::endl;

        auto line = input.lines()[span.start().row()];
        std::cerr << info << line << std::endl;
        std::cerr << std::string(span.start().offset() + info_len, ' ');
        std::cerr << std::string(
            span.end().offset() - span.start().offset() + 1, '^');
        std::cerr << "-- " << r.what() << std::endl;
    } else {
        auto [sinfo, sinfo_len] = gen_info(name, span.start(), level, true);
        auto [einfo, einfo_len] = gen_info(name, span.end(), level, false);
        auto info_len = sinfo_len > einfo_len ? sinfo_len : einfo_len;

        std::cerr << name << ": " << r.context() << std::endl;

        auto sline = input.lines()[span.start().row()];
        std::cerr << sinfo << sline << std::endl;
        std::cerr << std::string(span.start().offset() + info_len, ' ');
        std::cerr << std::string(sline.size() - span.start().offset(), '^');
        std::cerr << std::endl;

        auto eline = input.lines()[span.end().row()];
        std::cerr << einfo << eline << std::endl;
        std::cerr << std::string(info_len, ' ');
        std::cerr << std::string(span.end().offset() + 1, '^');
        std::cerr << "-- " << r.what() << std::endl;
    }
}

}  // namespace tinyc
