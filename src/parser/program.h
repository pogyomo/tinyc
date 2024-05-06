#ifndef TINYC_PARSER_PROGRAM_H_
#define TINYC_PARSER_PROGRAM_H_

#include <memory>
#include <optional>
#include <vector>

#include "decl.h"

namespace tinyc {

// A translate unit.
class Program {
public:
    Program(const std::vector<std::shared_ptr<Declaration>>& decls)
        : decls_(decls) {}

    inline const std::vector<std::shared_ptr<Declaration>>& decls() const {
        return decls_;
    }

private:
    const std::vector<std::shared_ptr<Declaration>> decls_;
};

}  // namespace tinyc

#endif  // TINYC_PARSER_PROGRAM_H_
