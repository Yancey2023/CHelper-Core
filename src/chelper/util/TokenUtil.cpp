//
// Created by Yancey on 2024/2/22.
//

#include "TokenUtil.h"

namespace CHelper::TokenUtil {

    size_t getIndex(const VectorView <Token> &tokens, size_t tokenIndex) {
        if (tokenIndex == 0) {
            return 0;
        } else if (tokenIndex == tokens.vector->size()) {
            return tokens.vector->at(tokenIndex - 1).getEndIndex();
        } else {
            return tokens.vector->at(tokenIndex).getStartIndex();
        }
    }

    size_t getStartIndex(const VectorView <Token> &tokens) {
        return getIndex(tokens, tokens.start);
    }

    size_t getEndIndex(const VectorView <Token> &tokens) {
        return getIndex(tokens, tokens.end);
    }

    std::string toString(const VectorView <Token> &tokens) {
        std::string result;
        tokens.forEach([&result](const CHelper::Token &token) {
            result.append(token.content);
        });
        return result;
    }

} // CHelper::TokenUtil