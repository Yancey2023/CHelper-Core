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
        stack.pop_back();
    }

    void next(const std::string &str) {
        pop();
        push(str);
    }

    void clear() {
        stack.clear();
    }

    std::string getStackTrace() {
        return StringUtil::join("\n", stack);
    }

} // CHelper::Profile