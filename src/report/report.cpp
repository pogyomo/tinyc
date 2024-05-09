#include "report.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "reportable.h"

namespace tinyc {

int digits(int n) {
    if (n == 0) {
        return 1;
    }
    int res = 0;
    while (n > 0) {
        res++;
        n /= 10;
    }
    return res;
}

void start_color(std::ostream& os, ReportableLevel level) {
    if (level == ReportableLevel::Error) {
        os << "\e[31m";
    } else {
        os << "\e[33m";
    }
}

void end_color(std::ostream& os) { os << "\e[0m"; }

std::string info(const std::string& filename, const std::string& context,
                 Position start, ReportableLevel level) {
    std::stringstream ss;
    ss << filename << ":" << start.row() << ":" << start.offset() << ": ";
    if (level == ReportableLevel::Error) {
        start_color(ss, level);
        ss << "error: ";
        end_color(ss);
    } else {
        start_color(ss, level);
        ss << "warning: ";
        end_color(ss);
    }
    ss << context;
    return ss.str();
}

void report(Context& ctx, const Reportable& r) {
    auto start = r.span().start();
    auto end = r.span().end();
    auto input = ctx.input_cache().fetch(r.span().id());
    auto name = input.name();
    std::cerr << info(name, r.context(), start, r.level()) << std::endl;
    if (start.row() == end.row()) {
        int width = digits(start.row());
        std::stringstream sep_start;
        sep_start << std::setfill('0') << std::setw(width);
        sep_start << "  " << start.row() << "|";
        std::stringstream sep;
        sep << std::string(width + 2, ' ') << "|";
        auto line_start = input.lines()[start.row()];

        std::cerr << sep_start.str() << line_start << std::endl;
        std::cerr << sep.str();
        std::cerr << std::string(start.offset(), ' ');
        start_color(std::cerr, r.level());
        std::cerr << '^' << std::string(end.offset() - start.offset(), '~');
        end_color(std::cerr);
        std::cout << " " << r.what() << std::endl;
    } else {
        int width = digits(std::max(start.row(), end.row()));
        std::stringstream sep_start;
        sep_start << std::setfill('0') << std::setw(width);
        sep_start << "  " << start.row() << "|";
        std::stringstream sep_end;
        sep_end << std::setfill('0') << std::setw(width);
        sep_end << "  " << end.row() << "|";
        std::stringstream sep;
        sep << std::string(width + 2, ' ') << "|";
        std::stringstream cont;
        cont << std::string(width + 2, ' ') << "...";
        auto line_start = input.lines()[start.row()];
        auto line_end = input.lines()[end.row()];

        std::cerr << sep_start.str() << line_start << std::endl;
        std::cerr << sep.str();
        std::cerr << std::string(start.offset(), ' ');
        start_color(std::cerr, r.level());
        std::cerr << '^';
        std::cerr << std::string(line_start.size() - start.offset() - 1, '~');
        end_color(std::cerr);
        std::cerr << std::endl;
        std::cerr << cont.str() << std::endl;

        std::cerr << sep_end.str() << line_end << std::endl;
        std::cerr << sep.str();
        start_color(std::cerr, r.level());
        std::cerr << std::string(end.offset() + 1, '~');
        end_color(std::cerr);
        std::cerr << " " << r.what() << std::endl;
    }
    std::cerr << std::endl;
}

}  // namespace tinyc
