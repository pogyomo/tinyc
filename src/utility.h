#ifndef TINYC_UTILITY_H_
#define TINYC_UTILITY_H_

#include <string>

namespace tinyc {

// Convert a character to printable string which appear in character or string
// literal.
std::string to_printable(char c);

}  // namespace tinyc

#endif  // TINYC_UTIL_H_
