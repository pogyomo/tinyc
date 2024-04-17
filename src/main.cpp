#include <fstream>
#include <iostream>
#include <vector>

#include "input.h"
#include "lexer.h"
#include "span.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 0;
    }
    std::ifstream ifs(argv[1]);
    tinyc::Input input(ifs);
    std::vector<tinyc::LexError> es;
    auto ts = tinyc::lex(input, es);
    if (!es.empty()) {
        for (auto e : es) {
            std::cout << e.msg() << std::endl;
        }
        return 0;
    }
    while (!ts.empty()) {
        auto [start_row, start_offset] = ts.token()->span().start();
        std::cout << start_row << ":" << start_offset << ":"
                  << ts.token()->debug() << std::endl;
        ts.advance();
    }
}
