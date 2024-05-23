//
// Created by Yancey on 2024/2/13.
//

#include "StringUtil.h"

namespace CHelper::StringUtil {

    std::string join(const std::string &joining, const std::vector<std::string> &strings) {
        if (HEDLEY_UNLIKELY(strings.empty())) {
            return {};
        }
        std::string result;
        result.append(strings[0]);
        std::for_each(strings.begin() + 1, strings.end(), [&](const auto &item) {
            result.append(joining);
            result.append(item);
        });
        return std::move(result);
    }

}// namespace CHelper::StringUtil