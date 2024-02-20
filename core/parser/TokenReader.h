//
// Created by Yancey666 on 2023/12/15.
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
        std::vector<size_t> indexQueue;

        explicit TokenReader(const std::vector<CHelper::Token> &tokenList);

        [[nodiscard]] bool ready() const;

        [[nodiscard]] const Token *read();

        bool skip();

        void skipAll();

        [[nodiscard]] const Token *next();

        [[nodiscard]] const Token *peek() const;

        void push();

        void pop();

        [[nodiscard]] size_t getAndPopLastIndex();

        void restore();

        VectorView<Token> collect();
//        [[nodiscard]] VectorView<Token> collect();
    };

} // CHelper

#endif //CHELPER_TOKENREADER_H
