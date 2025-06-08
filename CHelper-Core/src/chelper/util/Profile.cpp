//
// Created by Yancey on 2024/2/22.
//

#include <chelper/util/Profile.h>
#include <chelper/util/StringUtil.h>

namespace CHelper::Profile {

    std::vector<std::string> stack;

    void pop() {
        if (stack.empty()) {
            SPDLOG_ERROR("pop stack when stack is empty");
            return;
        }
        stack.pop_back();
    }

    void clear() {
        stack.clear();
    }

    void printAndClear(const std::exception &e) {
        SPDLOG_ERROR("{}\nstack trace:\n{}", e.what(), Profile::getStackTrace());
        stack.clear();
    }

    fmt::join_view<decltype(std::begin(stack)), decltype(std::end(stack)), char> getStackTrace() {
        return StringUtil::join(stack, "\n");
    }

}// namespace CHelper::Profile