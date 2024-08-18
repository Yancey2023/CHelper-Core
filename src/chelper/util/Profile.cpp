//
// Created by Yancey on 2024/2/22.
//

#include "Profile.h"
#include "StringUtil.h"

namespace CHelper::Profile {

    std::vector<std::wstring> stack;

    void pop() {
        if (stack.empty()) {
            CHELPER_ERROR(L"pop stack when stack is empty");
            return;
        }
        stack.pop_back();
    }

    void clear() {
        stack.clear();
    }

    void printAndClear(const std::exception &e) {
#if CHelperLogger == DEBUG || CHelperLogger == INFO || CHelperLogger == WARN || CHelperLogger == ERROR
        CHelper::Logger::error(std::string(e.what()));
#endif
        CHELPER_ERROR(L"stack trace:\n" + Profile::getStackTrace());
        stack.clear();
    }

    std::wstring getStackTrace() {
        return StringUtil::join(L"\n", stack);
    }

}// namespace CHelper::Profile