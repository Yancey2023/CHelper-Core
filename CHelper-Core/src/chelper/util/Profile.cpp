//
// Created by Yancey on 2024/2/22.
//

#include <chelper/util/Profile.h>
#include <chelper/util/StringUtil.h>

namespace CHelper::Profile {

#ifndef CHELPER_NO_FILESYSTEM
    std::vector<std::string> stack;
#endif

    void pop() {
#ifndef CHELPER_NO_FILESYSTEM
        if (stack.empty()) {
            SPDLOG_ERROR("pop stack when stack is empty");
            return;
        }
        stack.pop_back();
#endif
    }

    void clear() {
#ifndef CHELPER_NO_FILESYSTEM
        stack.clear();
#endif
    }

    void printAndClear(const std::exception &e) {
#ifndef CHELPER_NO_FILESYSTEM
        SPDLOG_ERROR("{}\nstack trace:\n{}", e.what(), StringUtil::join(stack, "\n"));
        stack.clear();
#endif
    }

}// namespace CHelper::Profile