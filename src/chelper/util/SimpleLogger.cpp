//
// Created by Yancey on 2023/12/1.
//

#include "SimpleLogger.h"

namespace CHelper::Logger {

    // 特化版本，用于处理 std::u16string
    std::string convertArg(std::u16string &u16string) {
        return utf8::utf16to8(u16string);
    }

    // 特化版本，用于处理 std::u16string
    std::string convertArg(const std::u16string &u16string) {
        return utf8::utf16to8(u16string);
    }

    // 特化版本，用于处理 char16_t
    std::string convertArg(const char16_t *u16string) {
        return utf8::utf16to8(std::u16string(u16string));
    }

}// namespace CHelper::Logger
