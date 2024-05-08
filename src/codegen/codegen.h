#ifndef TINYC_CODEGEN_CODEGEN_H_
#define TINYC_CODEGEN_CODEGEN_H_

#include <iostream>
#include <string>

#include "../context.h"

namespace tinyc {

void codegen(Context& ctx, std::istream& is, const std::string& name,
             std::ostream& os);

}  // namespace tinyc

#endif  // TINYC_CODEGEN_CODEGEN_H_
