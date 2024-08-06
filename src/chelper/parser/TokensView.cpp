//
// Created by Yancey on 2024-7-13.
//

#include "TokensView.h"

namespace CHelper {

    TokensView::TokensView(const std::shared_ptr<LexerResult> &lexerResult, size_t start, size_t end)
        : lexerResult(lexerResult),
          start(start),
          end(end) {
        startIndex = getIndex(start);
        endIndex = getIndex(end);
        cacheString = {lexerResult->content.c_str() + startIndex, endIndex - startIndex};
#if CHelperDebug == true
        if (HEDLEY_UNLIKELY(start > end)) {
            throw std::runtime_error("TokensView: wrong range: (" + std::to_string(start) + ", " + std::to_string(end) + ")");
        }
#endif
    }

    [[nodiscard]] bool TokensView::isEmpty() const {
        return start >= end;
    };

    [[nodiscard]] bool TokensView::hasValue() const {
        return start < end;
    };

    const Token &TokensView::operator[](size_t which) const {
        return lexerResult->allTokens[start + which];
    }

    [[nodiscard]] size_t TokensView::size() const {
        return end - start;
    }

    [[nodiscard]] bool TokensView::isAllWhitespace() const {
        return std::all_of(lexerResult->allTokens.begin() + static_cast<std::string::difference_type>(start),
                           lexerResult->allTokens.begin() + static_cast<std::string::difference_type>(end),
                           [](const auto &item) {
                               return item.type == TokenType::WHITE_SPACE;
                           });
    }

    void TokensView::forEach(std::function<void(const Token &token)> function) const {
        std::for_each(lexerResult->allTokens.begin() + static_cast<std::string::difference_type>(start),
                      lexerResult->allTokens.begin() + static_cast<std::string::difference_type>(end), 
                      std::move(function));
    }

    [[nodiscard]] size_t TokensView::getIndex(size_t tokenIndex) const {
        if (HEDLEY_UNLIKELY(tokenIndex == 0)) {
            return 0;
        } else if (HEDLEY_UNLIKELY(tokenIndex == lexerResult->allTokens.size())) {
            return lexerResult->allTokens[tokenIndex - 1].getEndIndex();
        } else {
            return lexerResult->allTokens[tokenIndex].getStartIndex();
        }
    }

    [[nodiscard]] size_t TokensView::getStartIndex() const {
        return startIndex;
    }

    [[nodiscard]] size_t TokensView::getEndIndex() const {
        return endIndex;
    }

    [[nodiscard]] std::string_view TokensView::toString() const {
        return cacheString;
    }

}// namespace CHelper