//
// Created by Yancey on 2023/12/15.
//

#ifndef CHELPER_TOKENREADER_H
#define CHELPER_TOKENREADER_H

#include "pch.h"
#include "../lexer/Token.h"
#include "../util/VectorView.h"

namespace CHelper {

    class TokenReader {
    public:
        const std::vector<CHelper::Token> &tokenList;
        size_t index = 0;
        std::vector<size_t> indexStack;

        explicit TokenReader(const std::vector<CHelper::Token> &tokenList);

        [[nodiscard]] bool ready() const;

        [[nodiscard]] const Token *peek() const;

        [[nodiscard]] const Token *read();

        [[nodiscard]] const Token *next();

        bool skip();

        bool skipWhitespace();

        void skipToLF();

        void push();

        void pop();

        [[nodiscard]] size_t getAndPopLastIndex();

        void restore();

        [[nodiscard]] VectorView<Token> collect();
    };

} // CHelper

#endif //CHELPER_TOKENREADER_H
