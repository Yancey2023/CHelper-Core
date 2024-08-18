//
// Created by Yancey on 2024-7-13.
//

#include "LexerResult.h"

namespace CHelper {

    LexerResult::LexerResult(std::wstring content,
                             std::vector<Token> allTokens)
        : content(std::move(content)),
          allTokens(std::move(allTokens)) {}

}// namespace CHelper
