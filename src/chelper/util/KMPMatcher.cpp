//
// Created by Yancey on 2024-07-24.
//

#include "KMPMatcher.h"

namespace CHelper {

    KMPMatcher::KMPMatcher(const std::wstring_view &pattern) : pattern(pattern) {
        next.reserve(this->pattern.length());
        size_t j = 0;
        for (size_t i = 1; i < this->pattern.length(); ++i) {
            while (j > 0 && this->pattern[i] != this->pattern[j]) {
                j = next[j - 1];
            }
            if (this->pattern[i] == this->pattern[j]) {
                ++j;
            }
            next.push_back(j);
        }
    }

    size_t KMPMatcher::match(const std::wstring &str) {
        if (pattern.empty()) {
            return 0;
        }
        size_t j = 0;
        for (size_t i = 0; i < str.length(); ++i) {
            while (j > 0 && str[i] != pattern[j]) {
                j = next[j - 1];
            }
            if (str[i] == pattern[j] && ++j == pattern.length()) {
                return i - j + 1;
            }
        }
        return std::wstring::npos;
    }

}// namespace CHelper