#include <fstream>
#include <iostream>

#include "input.h"
#include "lexer.h"
#include "span.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 0;
    }
    std::ifstream ifs(argv[1]);
    tinyc::Input input(ifs);
    auto ts = tinyc::lex(input);
    while (!ts.empty()) {
        auto [start_row, start_offset] = ts.token()->span().start();
        std::cout << start_row << ":" << start_offset << ":"
                  << ts.token()->debug() << std::endl;
        ts.advance();
    }
}
