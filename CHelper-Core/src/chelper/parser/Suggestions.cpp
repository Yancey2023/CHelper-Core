//
// Created by Yancey on 2024-03-12.
//

#include <chelper/parser/Suggestions.h>

namespace CHelper {

    namespace SuggestionsType {

        SuggestionsType suggestionsTypeMax = SuggestionsType::ID;

    }// namespace SuggestionsType


    Suggestions::Suggestions(SuggestionsType::SuggestionsType suggestionsType)
        : suggestionsType(suggestionsType) {}

    bool Suggestions::isFiltered() const {
        return mHashCode.has_value();
    }

    size_t Suggestions::hashCode() const {
        return mHashCode.value_or(0);
    }

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

    Suggestions Suggestions::singleSuggestion(SuggestionsType::SuggestionsType suggestionsType, Suggestion suggestion) {
        Suggestions suggestions(suggestionsType);
        suggestions.suggestions.push_back(std::move(suggestion));
        suggestions.markFiltered();
        return suggestions;
    }

    Suggestions Suggestions::singleSpaceSuggestion(Suggestion suggestion) {
        return singleSuggestion(SuggestionsType::SPACE, std::move(suggestion));
    }

    Suggestions Suggestions::singleSymbolSuggestion(Suggestion suggestion) {
        return singleSuggestion(SuggestionsType::SYMBOL, std::move(suggestion));
    }

    Suggestions Suggestions::singleLiteralSuggestion(Suggestion suggestion) {
        return singleSuggestion(SuggestionsType::LITERAL, std::move(suggestion));
    }

    std::vector<Suggestion> Suggestions::filter(std::vector<Suggestions> &suggestions) {
        // 过滤
        std::vector<Suggestions> filteredSuggestions;
        for (auto &item: suggestions) {
            item.filter();
            if (HEDLEY_LIKELY(std::all_of(
                        filteredSuggestions.begin(), filteredSuggestions.end(),
                        [&item](const Suggestions &item2) {
                            return item.hashCode() != item2.hashCode();
                        }))) {
                filteredSuggestions.push_back(item);
            }
        }
        // 根据优先级进行排序
        std::vector<Suggestion> result;
        size_t sum = 0;
        for (const auto &item: filteredSuggestions) {
            sum += item.suggestions.size();
        }
        result.reserve(sum);
        for (size_t suggestionsType = 0; suggestionsType <= SuggestionsType::suggestionsTypeMax; ++suggestionsType) {
            for (const auto &item: filteredSuggestions) {
                if (item.suggestionsType == suggestionsType) {
                    std::copy(item.suggestions.begin(), item.suggestions.end(), std::back_inserter(result));
                }
            }
        }
        return result;
    }

}// namespace CHelper