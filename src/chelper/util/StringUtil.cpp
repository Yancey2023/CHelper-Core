//
// Created by Yancey on 2024/2/13.
//

#include "StringUtil.h"

namespace CHelper::StringUtil {

    std::wstring join(const std::wstring &joining, const std::vector<std::wstring> &strings) {
        if (HEDLEY_UNLIKELY(strings.empty())) {
            return {};
        }
        std::wstring result;
        result.append(strings[0]);
        std::for_each(strings.begin() + 1, strings.end(), [&](const auto &item) {
            result.append(joining);
            result.append(item);
        });
        return result;
    }

}// namespace CHelper::StringUtil