#include "utility.h"

namespace tinyc {

std::string to_printable(char c) {
    if (c == 0x07) {
        return "\\a";
    } else if (c == 0x08) {
        return "\\b";
    } else if (c == 0x1B) {
        return "\\e";
    } else if (c == 0x0C) {
        return "\\f";
    } else if (c == 0x0A) {
        return "\\n";
    } else if (c == 0x0D) {
        return "\\r";
    } else if (c == 0x09) {
        return "\\t";
    } else if (c == 0x0B) {
        return "\\v";
    } else if (c == 0x5C) {
        return "\\\\";
    } else if (c == 0x27) {
        return "\\'";
    } else if (c == 0x22) {
        return "\\\"";
    } else if (c == 0x3F) {
        return "\\?";
    } else {
        return std::string(1, c);
    }
}

}  // namespace tinyc
