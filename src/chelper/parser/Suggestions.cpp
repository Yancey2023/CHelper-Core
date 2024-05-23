//
// Created by Yancey on 2024-03-12.
//

#include "Suggestions.h"

namespace CHelper {

    void Suggestions::markFiltered() {
        if (HEDLEY_UNLIKELY(isFiltered())) {
            return;
        }
        //生成哈希值
        size_t hashCode = 0;
        for (const auto &item: suggestions) {
            hashCode = 31 * hashCode + item.hashCode();
        }
        mHashCode = hashCode;
    }

    void Suggestions::filter() {
        if (HEDLEY_LIKELY(isFiltered())) {
            return;
        }
        //过滤
        std::vector<Suggestion> filteredSuggestions;
        for (const auto &item: suggestions) {
            bool flag = true;
            for (const auto &item2: filteredSuggestions) {
                if (HEDLEY_UNLIKELY(item2.equal(item))) {
                    flag = false;
                    break;
                }
            }
            if (HEDLEY_LIKELY(flag)) {
                filteredSuggestions.push_back(item);
            }
        }
        suggestions = std::move(filteredSuggestions);
        //生成哈希值
        markFiltered();
    }

    Suggestions Suggestions::singleSuggestion(Suggestion suggestion) {
        Suggestions suggestions;
        suggestions.suggestions.push_back(std::move(suggestion));
        suggestions.markFiltered();
        return suggestions;
    }

    std::vector<Suggestion> Suggestions::filter(std::vector<Suggestions> &suggestions) {
        //过滤
        std::vector<Suggestions> filteredSuggestions;
        for (auto &item: suggestions) {
            item.filter();
            if (HEDLEY_LIKELY(
                        std::all_of(filteredSuggestions.begin(), filteredSuggestions.end(),
                                    [&item](Suggestions &item2) {
                                        return item.hashCode() != item2.hashCode();
                                    }))) {
                filteredSuggestions.push_back(item);
            }
        }
        //收集
        std::vector<Suggestion> result;
        size_t sum = 0;
        for (const auto &item: filteredSuggestions) {
            sum += item.suggestions.size();
        }
        result.reserve(sum);
        for (const auto &item: filteredSuggestions) {
            for (const auto &item2: item.suggestions) {
                result.push_back(item2);
            }
        }
        return result;
    }

}// namespace CHelper