//
// Created by Yancey on 2024-7-13.
//

#pragma once

#ifndef CHELPER_LEXERRESULT_H
#define CHELPER_LEXERRESULT_H

#include "Token.h"
#include "pch.h"

namespace CHelper {

    class LexerResult {
    public:
        const std::wstring content;
        const std::vector<Token> allTokens;

        LexerResult(std::wstring content, std::vector<Token> allTokens);
    };

}// namespace CHelper

#endif//CHELPER_LEXERRESULT_H
