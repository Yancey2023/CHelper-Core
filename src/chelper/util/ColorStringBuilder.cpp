//
// Created by Yancey on 2024/2/22.
//

#include "ColorStringBuilder.h"

namespace CHelper {

#if CHelperAndroid == true

    ColorStringBuilder &ColorStringBuilder::append(const std::string &str) {
        content.append(str);
        return *this;
    }

    ColorStringBuilder &ColorStringBuilder::normal(const std::string &str) {
        return append(str);
    }

    ColorStringBuilder &ColorStringBuilder::grey(const std::string &str) {
        return append(str);
    }

    ColorStringBuilder &ColorStringBuilder::red(const std::string &str) {
        return append(str);
    }

    ColorStringBuilder &ColorStringBuilder::green(const std::string &str) {
        return append(str);
    }

    ColorStringBuilder &ColorStringBuilder::yellow(const std::string &str) {
        return append(str);
    }

    ColorStringBuilder &ColorStringBuilder::blue(const std::string &str) {
        return append(str);
    }

    ColorStringBuilder &ColorStringBuilder::purple(const std::string &str) {
        return append(str);
    }

    ColorStringBuilder &ColorStringBuilder::cyan(const std::string &str) {
        return append(str);
    }

    std::string ColorStringBuilder::build() {
        return std::move(content);
    }

#else

    namespace Color {
        const std::string NORMAL = "\033[0m";
        const std::string GREY = "\033[30m";
        const std::string RED = "\033[31m";
        const std::string GREEN = "\033[32m";
        const std::string YELLOW = "\033[33m";
        const std::string BLUE = "\033[34m";
        const std::string PURPLE = "\033[35m";
        const std::string CYAN = "\033[36m";
    }

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

#endif

} // CHelper

