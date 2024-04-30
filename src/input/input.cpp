#include "input.h"

#include <istream>
#include <string>
#include <vector>

namespace tinyc {

std::vector<std::string> collect_lines(std::istream& is) {
    std::string line;
    std::vector<std::string> res;
    while (std::getline(is, line)) {
        res.push_back(line);
    }
    return res;
}

Input::Input(std::istream& is, int id) : id_(id), lines_(collect_lines(is)) {}

}  // namespace tinyc
