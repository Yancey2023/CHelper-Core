//
// Created by Yancey on 2024/2/22.
//

#include "Profile.h"
#include "StringUtil.h"

namespace CHelper::Profile {

    std::vector<std::string> stack;

    void push(const std::string &str) {
        stack.push_back(str);
    }

    void pop() {
        if (stack.empty()) {
            return;
        }
        stack.pop_back();
    }

    void next(const std::string &str) {
        pop();
        push(str);
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