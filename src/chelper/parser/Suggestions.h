//
// Created by Yancey on 2024-03-12.
//

#pragma once

#ifndef CHELPER_SUGGESTIONS_H
#define CHELPER_SUGGESTIONS_H

#include "Suggestion.h"

namespace CHelper {

    namespace SuggestionsType {

        enum SuggestionsType : uint8_t {
            WHITESPACE = 0,
            SYMBOL = 1,
            LITERAL = 2,
            ID = 3
        };

        extern SuggestionsType suggestionsTypeMax;
    }// namespace SuggestionsType

    //为了更快地过滤补全建议，要对不同的建议组合进行一些标记
    class Suggestions {
    public:
        SuggestionsType::SuggestionsType suggestionsType;
        std::vector<Suggestion> suggestions;

    private:
        std::optional<size_t> mHashCode;

    public:
        explicit Suggestions(SuggestionsType::SuggestionsType suggestionsType);

        bool isFiltered();

        size_t hashCode();

        void markFiltered();

        void filter();

        static Suggestions singleSuggestion(SuggestionsType::SuggestionsType suggestionsType, Suggestion suggestion);

        static Suggestions singleWhitespaceSuggestion(Suggestion suggestion);

        static Suggestions singleSymbolSuggestion(Suggestion suggestion);

        static Suggestions singleLiteralSuggestion(Suggestion suggestion);

        static std::vector<Suggestion> filter(std::vector<Suggestions> &suggestions);
    };

}// namespace CHelper

#endif//CHELPER_SUGGESTIONS_H
