//
// Created by Yancey on 2024-03-12.
//

#pragma once

#ifndef CHELPER_SUGGESTIONS_H
#define CHELPER_SUGGESTIONS_H

#include "Suggestion.h"

namespace CHelper {

    //为了更快地过滤补全建议，要对不同的建议组合进行一些标记
    class Suggestions {
    public:
        std::vector<Suggestion> suggestions;

    private:
        std::optional<size_t> mHashCode;

    public:
        Suggestions() = default;

        bool isFiltered() {
            return mHashCode.has_value();
        }

        size_t hashCode() {
            return mHashCode.value_or(0);
        }

        void markFiltered();

        void filter();

        static Suggestions singleSuggestion(Suggestion suggestion);

        static std::vector<Suggestion> filter(std::vector<Suggestions> &suggestions);
    };

}// namespace CHelper

#endif//CHELPER_SUGGESTIONS_H
