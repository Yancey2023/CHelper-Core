//
// Created by Yancey666 on 2023/11/13.
//

#include "HashUtil.h"

namespace CHelper::HashUtil {

    static std::size_t hashCode() {
        return 0;
    }

    template<typename T, typename... Args>
    static int hashCode(const T &first, const Args &...args) {
        int a = std::hash<T>{}(first);
        int b = hashCode(args...);
        if (b == 0) {
            return a;
        } else {
            return 31 * a + b;
        }
    }

    template<typename T1, typename T2>
    std::size_t PairHash::operator()(const std::pair<T1, T2> &p) const {
        return hashCode(p.first, p.second);
    }

    std::size_t ErrorReasonHash::operator()(const ErrorReason &errorReason) const {
        return hashCode(errorReason.errorReason, errorReason.tokens.start, errorReason.tokens.end);
    }

} // CHelper::HashUtil