//
// Created by Yancey on 2024/2/22.
//

#include "TokenUtil.h"

namespace CHelper::TokenUtil {

    size_t getIndex(const VectorView <Token> &tokens, size_t tokenIndex) {
        if (HEDLEY_UNLIKELY(tokenIndex == 0)) {
            return 0;
        } else if (HEDLEY_UNLIKELY(tokenIndex == tokens.vector->size())) {
            return tokens.vector->at(tokenIndex - 1).getEndIndex();
        } else {
            return tokens.vector->at(tokenIndex).getStartIndex();
        }
    }

    std::string toString(const VectorView <Token> &tokens) {
        std::string result;
        tokens.forEach([&result](const CHelper::Token &token) {
            result.append(token.content);
        });
        return std::move(result);
    }

} // CHelper::TokenUtil