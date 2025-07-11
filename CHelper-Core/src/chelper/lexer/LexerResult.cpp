//
// Created by Yancey on 2024-7-13.
//

#include <chelper/lexer/LexerResult.h>

namespace CHelper {

    LexerResult::LexerResult(std::u16string content,
                             std::vector<Token> allTokens)
        : content(std::move(content)),
          allTokens(std::move(allTokens)) {}

    [[nodiscard]] size_t LexerResult::getIndex(size_t tokenIndex) const {
        if (HEDLEY_UNLIKELY(tokenIndex == 0)) {
            return 0;
        } else if (HEDLEY_UNLIKELY(tokenIndex == allTokens.size())) {
            return allTokens[tokenIndex - 1].getEndIndex();
        } else {
            return allTokens[tokenIndex].getStartIndex();
        }
    }

}// namespace CHelper
