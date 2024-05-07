#include <fstream>
#include <iostream>

#include "context.h"
#include "parser/error.h"
#include "parser/parser.h"
#include "report/report.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 0;
    }

    tinyc::Context ctx;
    std::fstream fs(argv[1]);
    std::vector<tinyc::ParseError> es;
    auto prog = parse(ctx, fs, argv[1], es);
    if (es.empty()) {
        if (prog.decls().empty()) {
            std::cout << "result is empty" << std::endl;
        } else {
            for (const auto& decl : prog.decls()) {
                std::cout << decl->debug() << std::endl;
            }
        }
    } else {
        for (auto& e : es) {
            tinyc::report(ctx, e);
        }
    }
}
