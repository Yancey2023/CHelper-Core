//
// Created by Yancey666 on 2023/11/13.
//

#include "HashUtil.h"

namespace CHelper::HashUtil {

    template<typename T1, typename T2>
    std::size_t PairHash::operator()(const std::pair<T1, T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // 主要的想法是将第一个元素的哈希值移位，然后与第二个元素的哈希值进行异或操作。
        return h1 ^ h2;
    }

} // CHelper::HashUtil