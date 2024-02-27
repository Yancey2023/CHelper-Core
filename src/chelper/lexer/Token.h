//
// Created by Yancey on 2023/11/6.
//

#ifndef CHELPER_TOKEN_H
#define CHELPER_TOKEN_H

#include "pch.h"
#include "LexerPos.h"

namespace CHelper {

    namespace TokenType {

        enum TokenType : std::uint8_t {
            STRING,
            NUMBER,
            SYMBOL,
            WHITE_SPACE,
            LF
        };

        std::string getName(TokenType tokenType);
    }


    class Token {
    public:
        const TokenType::TokenType type;
        const LexerPos pos;
        const std::string content;

        Token(TokenType::TokenType type, LexerPos pos, std::string content);

        [[nodiscard]] size_t getStartIndex() const;

        [[nodiscard]] size_t getEndIndex() const;
    };

}

std::ostream &operator<<(std::ostream &os, const CHelper::TokenType::TokenType &tokenType);

std::ostream &operator<<(std::ostream &os, const CHelper::Token &token);

#endif //CHELPER_TOKEN_H
