//
// Created by Yancey on 2024/2/13.
//

#pragma once

#ifndef CHELPER_STRINGUTIL_H
#define CHELPER_STRINGUTIL_H

#include <string>
#include <vector>

namespace CHelper::StringUtil {

    template<typename T>
    std::basic_string<T> join(const std::basic_string<T> &joining, const std::vector<std::basic_string<T>> &strings) {
        if (HEDLEY_UNLIKELY(strings.empty())) {
            return {};
        }
        std::basic_string<T> result;
        result.append(strings[0]);
        std::for_each(strings.begin() + 1, strings.end(), [&](const auto &item) {
            result.append(joining);
            result.append(item);
        });
        return result;
    }

    template<typename T>
    std::basic_string<T> join(const T *joining, const std::vector<std::basic_string<T>> &strings) {
        return join(std::basic_string<T>(joining), strings);
    }

}// namespace CHelper::StringUtil

#endif//CHELPER_STRINGUTIL_H