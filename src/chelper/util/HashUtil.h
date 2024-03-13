//
// Created by Yancey on 2024/3/10.
//

#pragma once

#ifndef CHELPER_HASHUTIL_H
#define CHELPER_HASHUTIL_H

namespace CHelper::HashUtil {

    template<class T, class S>
    static size_t combineHash(const T &a, const S &b) {
        size_t h1 = std::hash<T>{}(a);
        size_t h2 = std::hash<S>{}(b);
        return 31 * h1 + h2;
    }

} // CHelper::HashUtil

#endif //CHELPER_HASHUTIL_H
