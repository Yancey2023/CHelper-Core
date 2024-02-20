//
// Created by Yancey666 on 2023/11/6.
//

#ifndef CHELPER_TOKEN_H
#define CHELPER_TOKEN_H

#include "pch.h"
#include "LexerPos.h"

namespace CHelper {

    namespace TokenType {

        enum TokenType {
            STRING,
            NUMBER,
            SYMBOL,
            LF
        };

        std::string getName(TokenType tokenType);
    }


    class Token {
    public:
        const TokenType::TokenType type;
        const bool whiteSpace; //前面有没有空格
        const LexerPos pos;
        const std::string content;

        Token(TokenType::TokenType type, bool whiteSpace, LexerPos pos, std::string content);
    };

}

std::ostream &operator<<(std::ostream &os, const CHelper::TokenType::TokenType &tokenType);

std::ostream &operator<<(std::ostream &os, const CHelper::Token &token);

#endif //CHELPER_TOKEN_H
