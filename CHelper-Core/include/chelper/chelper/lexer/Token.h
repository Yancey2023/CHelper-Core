//
// Created by Yancey on 2023/11/6.
//

#pragma once

#ifndef CHELPER_TOKEN_H
#define CHELPER_TOKEN_H

#include "LexerPos.h"
#include <pch.h>

namespace CHelper {

    namespace TokenType {

        enum TokenType : uint8_t {
            STRING,
            NUMBER,
            SYMBOL,
            WHITE_SPACE,
            LF
        };

        std::u16string getName(TokenType tokenType);

    }// namespace TokenType


    class Token {
    public:
        const TokenType::TokenType type;
        const LexerPos pos;
        const std::u16string_view content;

        Token(TokenType::TokenType type, LexerPos pos, const std::u16string_view &content);

        [[nodiscard]] size_t getStartIndex() const;

        [[nodiscard]] size_t getEndIndex() const;
    };

}// namespace CHelper

#ifdef CHelperTest
std::ostream &operator<<(std::ostream &os, const CHelper::TokenType::TokenType &tokenType);

std::ostream &operator<<(std::ostream &os, const CHelper::Token &token);
#endif

CODEC_ENUM(CHelper::TokenType::TokenType, uint8_t)

#endif//CHELPER_TOKEN_H
