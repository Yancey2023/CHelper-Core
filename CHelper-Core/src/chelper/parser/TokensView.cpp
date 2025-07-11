//
// Created by Yancey on 2024-7-13.
//

#include <chelper/parser/TokensView.h>

namespace CHelper {

    TokensView::TokensView(const std::shared_ptr<LexerResult> &lexerResult, size_t start, size_t end)
        : lexerResult(lexerResult),
          start(start),
          end(end) {
        startIndex = lexerResult->getIndex(start);
        endIndex = lexerResult->getIndex(end);
        cacheString = {lexerResult->content.c_str() + startIndex, endIndex - startIndex};
#ifdef CHelperDebug
        if (HEDLEY_UNLIKELY(start > end)) {
            throw std::runtime_error(fmt::format("TokensView: wrong range: ({}, {})", start, end));
        }
#endif
    }

    [[nodiscard]] bool TokensView::isEmpty() const {
        return start >= end;
    }

    [[nodiscard]] bool TokensView::hasValue() const {
        return start < end;
    }

    const Token &TokensView::operator[](size_t which) const {
        return lexerResult->allTokens[start + which];
    }

    [[nodiscard]] size_t TokensView::size() const {
        return end - start;
    }

    [[nodiscard]] bool TokensView::isAllSpace() const {
        return std::all_of(lexerResult->allTokens.begin() + static_cast<std::u16string::difference_type>(start),
                           lexerResult->allTokens.begin() + static_cast<std::u16string::difference_type>(end),
                           [](const auto &item) {
                               return item.type == TokenType::SPACE;
                           });
    }

    void TokensView::forEach(std::function<void(const Token &token)> function) const {
        std::for_each(lexerResult->allTokens.begin() + static_cast<std::u16string::difference_type>(start),
                      lexerResult->allTokens.begin() + static_cast<std::u16string::difference_type>(end),
                      std::move(function));
    }

    [[nodiscard]] std::u16string_view TokensView::toString() const {
        return cacheString;
    }

}// namespace CHelper