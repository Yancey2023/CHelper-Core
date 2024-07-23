#include <iostream>
#include <string_view>
#include <vector>

namespace CHelper {

    class KMPMatcher {
    private:
        std::string_view pattern;
        std::vector<size_t> next;

    public:
        explicit KMPMatcher(std::string_view pattern) : pattern(pattern) {
            next.resize(pattern.length());
            size_t j = 0;
            for (size_t i = 1; i < pattern.length(); ++i) {
                while (j > 0 && pattern[i] != pattern[j]) {
                    j = next[j - 1];
                }
                if (pattern[i] == pattern[j]) {
                    ++j;
                }
                next[i] = j;
            }
        }

        size_t match(const std::string &str) {
            size_t j = 0;
            for (size_t i = 0; i < str.length(); ++i) {
                while (j > 0 && str[i] != pattern[j]) {
                    j = next[j - 1];
                }
                if (str[i] == pattern[j]) {
                    ++j;
                }
                if (j == pattern.length()) {
                    return i - j + 1;
                }
            }
            return std::string::npos;
        }
    };

}// namespace CHelper