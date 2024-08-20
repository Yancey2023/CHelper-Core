//
// Created by Yancey on 2024-7-13.
//

#include <chelper/lexer/LexerResult.h>

namespace CHelper {

    LexerResult::LexerResult(std::u16string content,
                             std::vector<Token> allTokens)
        : content(std::move(content)),
          allTokens(std::move(allTokens)) {}

}// namespace CHelper
