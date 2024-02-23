//
// Created by Yancey on 2024/2/22.
//

#include "ColorStringBuilder.h"

namespace CHelper {

    ColorStringBuilder &ColorStringBuilder::append(const std::string &color, const std::string &str) {
        content.append(color).append(str);
        return *this;
    }

    ColorStringBuilder &ColorStringBuilder::normal(const std::string &str) {
        return append(Color::NORMAL, str);
    }

    ColorStringBuilder &ColorStringBuilder::grey(const std::string &str) {
        return append(Color::GREY, str);
    }

    ColorStringBuilder &ColorStringBuilder::red(const std::string &str) {
        return append(Color::RED, str);
    }

    ColorStringBuilder &ColorStringBuilder::green(const std::string &str) {
        return append(Color::GREEN, str);
    }

    ColorStringBuilder &ColorStringBuilder::yellow(const std::string &str) {
        return append(Color::YELLOW, str);
    }

    ColorStringBuilder &ColorStringBuilder::blue(const std::string &str) {
        return append(Color::BLUE, str);
    }

    ColorStringBuilder &ColorStringBuilder::purple(const std::string &str) {
        return append(Color::PURPLE, str);
    }

    ColorStringBuilder &ColorStringBuilder::cyan(const std::string &str) {
        return append(Color::CYAN, str);
    }

    std::string ColorStringBuilder::build() {
        content.append(Color::NORMAL);
        return std::move(content);
    }

} // CHelper

