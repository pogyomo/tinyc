#ifndef TINYC_REPORT_COLOR_H_
#define TINYC_REPORT_COLOR_H_

#include <sstream>
#include <string>

namespace tinyc {

enum class FgColor {
    None,
    Red,
    Yellow,
};

class ColoredString {
public:
    // Construct a new `ColoredString` without color.
    ColoredString(const std::string& s) : s_(s), fg_color_(FgColor::None) {}

    // Returns original string's size.
    int size() { return s_.size(); }

    // Change fg color to red.
    void set_fg_red() { fg_color_ = FgColor::Red; }

    // Change fg color to yellow.
    void set_fg_yellow() { fg_color_ = FgColor::Yellow; }

    // Returns colored string.
    std::string str() {
        std::stringstream ss;
        switch (fg_color_) {
            case FgColor::Red:
                ss << "\e[31m";
                break;
            case FgColor::Yellow:
                ss << "\e[33m";
                break;
            default:
                break;
        }
        ss << s_ << "\e[0m";
        return ss.str();
    }

private:
    FgColor fg_color_;
    std::string s_;
};

}  // namespace tinyc

#endif  // TINYC_REPORT_COLOR_H_
