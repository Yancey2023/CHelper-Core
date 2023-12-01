//
// Created by Yancey666 on 2023/11/6.
//

#ifndef CHELPER_TOKEN_H
#define CHELPER_TOKEN_H

#include "pch.h"
#include "Pos.h"

namespace CHelper {

    namespace TokenType {

        enum TokenType {
            STRING,
            NUMBER,
            SYMBOL
        };
    }

    std::ostream &operator<<(std::ostream &os, const TokenType::TokenType &tokenType);

    class Token {
    public:
        const TokenType::TokenType type;
        const bool whiteSpace; //前面有没有空格
        const Pos pos;
        const std::string content;

        Token(TokenType::TokenType type, bool whiteSpace, Pos pos, std::string content);
    };

    std::ostream &operator<<(std::ostream &os, const Token &token);

}

#endif //CHELPER_TOKEN_H
