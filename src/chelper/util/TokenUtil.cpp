//
// Created by Yancey on 2024/2/22.
//

#include "TokenUtil.h"

namespace CHelper::TokenUtil {

    size_t getStartIndex(VectorView<Token> tokens) {
        if (tokens.start == 0) {
            return 0;
        } else if (tokens.start == tokens.vector->size()) {
            return tokens.vector->at(tokens.start - 1).getEndIndex();
        } else {
            return tokens.vector->at(tokens.start).getStartIndex();
        }
    }

    size_t getEndIndex(VectorView<Token> tokens) {
        if (tokens.end == 0) {
            return 0;
        } else {
            return tokens.vector->at(tokens.end - 1).getEndIndex();
        }
    }

    std::string toString(VectorView<Token> tokens) {
        std::string result;
        tokens.for_each([&result](const CHelper::Token &token) {
            result.append(token.content);
        });
        return result;
    }

} // CHelper::TokenUtil