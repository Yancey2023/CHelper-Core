//
// Created by Yancey on 2024/2/22.
//

#include "Profile.h"
#include "StringUtil.h"

namespace CHelper::Profile {

    std::vector<std::string> stack;

    void next(const std::string &str) {
        pop();
        push(str);
    }

    std::string getStackTrace() {
        return StringUtil::join("\n", stack);
    }

} // CHelper::Profile