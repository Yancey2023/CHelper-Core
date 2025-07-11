//
// Created by Yancey on 2024-03-12.
//

#include <chelper/auto_suggestion/Suggestions.h>

namespace CHelper::AutoSuggestion {

    void Suggestions::addSuggestion(std::vector<Suggestion> &suggestions, Suggestion &&suggestion) {
        if (seenSuggestionHashes.insert(suggestion.hashCode()).second) {
            suggestions.push_back(std::move(suggestion));
        }
    }

    void Suggestions::addSpaceSuggestion(Suggestion &&suggestion) {
        addSuggestion(spaceSuggestions, std::move(suggestion));
    }

    void Suggestions::addSymbolSuggestion(Suggestion &&suggestion) {
        addSuggestion(symbolSuggestions, std::move(suggestion));
    }

    void Suggestions::addLiteralSuggestion(Suggestion &&suggestion) {
        addSuggestion(literalSuggestions, std::move(suggestion));
    }

    void Suggestions::reserveIdSuggestion(size_t size) {
        idSuggestions.reserve(idSuggestions.size() + size);
    }

    void Suggestions::addIdSuggestion(Suggestion &&suggestion) {
        addSuggestion(idSuggestions, std::move(suggestion));
    }

    void Suggestions::combine(Suggestions &suggestions, const std::function<bool(Suggestion &suggestion)> &function) {
        spaceSuggestions.reserve(spaceSuggestions.size() + suggestions.spaceSuggestions.size());
        symbolSuggestions.reserve(symbolSuggestions.size() + suggestions.symbolSuggestions.size());
        literalSuggestions.reserve(literalSuggestions.size() + suggestions.literalSuggestions.size());
        idSuggestions.reserve(idSuggestions.size() + suggestions.idSuggestions.size());
        for (auto &item: suggestions.spaceSuggestions) {
            if (function(item)) {
                addSuggestion(spaceSuggestions, std::move(item));
            }
        }
        for (auto &item: suggestions.symbolSuggestions) {
            if (function(item)) {
                addSuggestion(symbolSuggestions, std::move(item));
            }
        }
        for (auto &item: suggestions.literalSuggestions) {
            if (function(item)) {
                addSuggestion(literalSuggestions, std::move(item));
            }
        }
        for (auto &item: suggestions.idSuggestions) {
            if (function(item)) {
                addSuggestion(idSuggestions, std::move(item));
            }
        }
    }

    std::vector<Suggestion> Suggestions::collect() {
        std::vector<Suggestion> result;
        result.insert(result.end(), std::make_move_iterator(spaceSuggestions.begin()), std::make_move_iterator(spaceSuggestions.end()));
        result.insert(result.end(), std::make_move_iterator(symbolSuggestions.begin()), std::make_move_iterator(symbolSuggestions.end()));
        result.insert(result.end(), std::make_move_iterator(literalSuggestions.begin()), std::make_move_iterator(literalSuggestions.end()));
        result.insert(result.end(), std::make_move_iterator(idSuggestions.begin()), std::make_move_iterator(idSuggestions.end()));
        return result;
    }

}// namespace CHelper::AutoSuggestion