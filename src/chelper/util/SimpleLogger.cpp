//
// Created by Yancey on 2023/12/1.
//

#include "SimpleLogger.h"

namespace CHelper::Logger {

    // 特化版本，用于处理 std::wstring
    std::string convertArg(std::wstring &wstring) {
        return wstring2string(wstring);
    }

    // 特化版本，用于处理 std::wstring
    std::string convertArg(const std::wstring &wstring) {
        return wstring2string(wstring);
    }

    // 特化版本，用于处理 wchar_t
    std::string convertArg(const wchar_t *wstring) {
        return wstring2string(wstring);
    }

}// namespace CHelper::Logger
