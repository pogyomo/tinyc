#include <fstream>
#include <iostream>
#include <vector>

#include "context.h"
#include "lexer/error.h"
#include "lexer/lexer.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 0;
    }

    tinyc::Context ctx;
    std::fstream fs(argv[1]);
    std::vector<tinyc::LexError> es;
    auto ts = tinyc::lex(ctx, fs, es);
    while (!ts.eos()) {
        std::cout << ts.token()->debug() << std::endl;
        ts.advance();
    }
}
