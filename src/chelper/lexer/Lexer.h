//
// Created by Yancey on 2023/11/6.
//

#ifndef CHELPER_LEXER_H
#define CHELPER_LEXER_H

#include "pch.h"
#include "StringReader.h"
#include "Token.h"

namespace CHelper {

    class Lexer {
        StringReader stringReader;

    public:
        explicit Lexer(const std::string &content, const std::string &filename);

        explicit Lexer(StringReader stringReader);

        std::vector<Token> lex();

    private:
        TokenType::TokenType nextTokenType();

        Token nextTokenNumber();

        Token nextTokenSymbol();

        Token nextTokenString();

        Token nextTokenWhiteSpace();

        Token nextTokenLF();
    };

}

#endif //CHELPER_LEXER_H
