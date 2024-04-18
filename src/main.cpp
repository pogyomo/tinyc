#include <fstream>
#include <iostream>
#include <vector>

#include "input/cache.h"
#include "lexer/lexer.h"
#include "report/report.h"
#include "span.h"

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
    while (!ts.eos()) {
        auto [start_row, start_offset] = ts.token()->span().start();
        auto [end_row, end_offset] = ts.token()->span().end();
        std::cout << start_row << ":" << start_offset << ":" << end_row << ":"
                  << end_offset << ": " << ts.token()->debug() << std::endl;
        ts.advance();
    }
}
