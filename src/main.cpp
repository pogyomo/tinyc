#include <fstream>
#include <iostream>

#include "context.h"
#include "parser/parser.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 0;
    }

    tinyc::Context ctx;
    std::fstream fs(argv[1]);
    auto prog = parse(ctx, fs, argv[1]);
    if (prog.has_value()) {
        for (const auto& decl : prog->decls()) {
            std::cout << decl->debug() << std::endl;
        }
    }
}
