#include <fstream>
#include <iostream>
#include <vector>

#include "input/cache.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "report/report.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 0;
    }

    tinyc::InputCache cache;
    tinyc::Reporter report(cache);

    std::ifstream ifs(argv[1]);
    auto id = cache.cache(ifs, argv[1]);

    std::vector<tinyc::LexError> es;
    auto ts = tinyc::lex(cache, id, es);
    if (!es.empty()) {
        for (auto e : es) {
            report.report(e);
        }
        return 0;
    }

    try {
        auto expr = tinyc::parse_expr(ts);
        std::cout << expr->debug() << std::endl;
    } catch (tinyc::ParseError e) {
        report.report(e);
        return 0;
    }
}
