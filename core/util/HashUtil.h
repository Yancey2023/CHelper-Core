//
// Created by Yancey666 on 2023/11/13.
//

#ifndef CHELPER_HASHUTIL_H
#define CHELPER_HASHUTIL_H

#include "pch.h"

namespace CHelper::HashUtil {

    struct PairHash {
        template<typename T1, typename T2>
        std::size_t operator()(const std::pair<T1, T2> &p) const;
    };

} // CHelper::HashUtil

#endif //CHELPER_HASHUTIL_H
