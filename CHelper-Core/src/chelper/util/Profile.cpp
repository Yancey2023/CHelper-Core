//
// Created by Yancey on 2024/2/22.
//

#include <chelper/util/Profile.h>
#include <chelper/util/StringUtil.h>

namespace CHelper::Profile {

    std::vector<std::string> stack;

    void pop() {
        if (stack.empty()) {
            CHELPER_ERROR("pop stack when stack is empty");
            return;
        }
        stack.pop_back();
    }

    void clear() {
        stack.clear();
    }

    void printAndClear(const std::exception &e) {
        CHELPER_ERROR(std::string(e.what()) + "\nstack trace:\n" + Profile::getStackTrace());
        stack.clear();
    }

    std::string getStackTrace() {
        return StringUtil::join("\n", stack);
    }

}// namespace CHelper::Profile