//
// Created by Yancey666 on 2024/2/13.
//

#include <algorithm>
#include "StringUtil.h"

namespace CHelper::StringUtil {

    std::string join(const std::string &joining, const std::vector<std::string> &strings) {
        std::string result;
        bool isFirst = true;
        std::for_each(strings.begin(), strings.end(), [&](const auto &item) {
            if (isFirst) {
                isFirst = false;
            } else {
                result.append(joining);
            }
            result.append(item);
        });
        return result;
    }

    bool isStartOf(std::string_view a, std::string_view b) {
        return a.length() >= b.length() && a.substr(0, b.length()) == b;
    }

} //CHelper::StringUtil