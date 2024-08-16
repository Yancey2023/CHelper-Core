//
// Created by Yancey on 2024-7-13.
//

#pragma once

#ifndef CHELPER_TOKENSVIEW_H
#define CHELPER_TOKENSVIEW_H

#include "../lexer/LexerResult.h"
#include "../lexer/Token.h"
#include "pch.h"

namespace CHelper {

    class TokensView {
    public:
        std::shared_ptr<LexerResult> lexerResult;
        size_t start, end;
        size_t startIndex, endIndex;

    private:
        std::wstring_view cacheString;

    public:
        TokensView(const std::shared_ptr<LexerResult> &lexerResult, size_t start, size_t end);

        [[nodiscard]] bool isEmpty() const;

        [[nodiscard]] bool hasValue() const;

        const Token &operator[](size_t which) const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] bool isAllWhitespace() const;

        void forEach(std::function<void(const Token &token)> function) const;

        [[nodiscard]] size_t getIndex(size_t tokenIndex) const;

        [[nodiscard]] size_t getStartIndex() const;

        [[nodiscard]] size_t getEndIndex() const;

        [[nodiscard]] std::wstring_view toString() const;
    };

}// namespace CHelper

#endif