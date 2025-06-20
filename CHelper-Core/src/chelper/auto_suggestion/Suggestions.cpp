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

    std::vector<Suggestion> Suggestions::collect() {
        std::vector<Suggestion> result;
        result.insert(result.end(), std::make_move_iterator(spaceSuggestions.begin()), std::make_move_iterator(spaceSuggestions.end()));
        result.insert(result.end(), std::make_move_iterator(symbolSuggestions.begin()), std::make_move_iterator(symbolSuggestions.end()));
        result.insert(result.end(), std::make_move_iterator(literalSuggestions.begin()), std::make_move_iterator(literalSuggestions.end()));
        result.insert(result.end(), std::make_move_iterator(idSuggestions.begin()), std::make_move_iterator(idSuggestions.end()));
        return result;
    }

}// namespace CHelper