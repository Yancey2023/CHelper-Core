//
// Created by Yancey on 2023/12/15.
//

#include "TokenReader.h"

namespace CHelper {

    TokenReader::TokenReader(const std::shared_ptr<std::vector<Token>> &tokenList)
            : tokenList(tokenList) {}

    bool TokenReader::ready() const {
        return index < tokenList->size();
    }

    const Token *TokenReader::peek() const {
        if (!ready()) {
            return nullptr;
        }
        return &(*tokenList)[index];
    }

    const Token *TokenReader::read() {
        const Token *result = peek();
        if (result != nullptr) {
            skip();
        }
        return result;
    }

    const Token *TokenReader::next() {
        skip();
        return peek();
    }

    bool TokenReader::skip() {
        if (!ready()) {
            return false;
        }
        index++;
        return true;
    }

    bool TokenReader::skipWhitespace() {
        bool flag = false;
        while (ready() && peek()->type == TokenType::WHITE_SPACE) {
            skip();
            flag = true;
        }
        return flag;
    }

    void TokenReader::skipToLF() {
        while (ready() && peek()->type != TokenType::LF) {
            skip();
        }
    }

    /**
     * 将当前指针加入栈中
     */
    void TokenReader::push() {
        indexStack.push_back(index);
    }

    /**
     * 从栈中移除指针，不恢复指针
     */
    void TokenReader::pop() {
        indexStack.pop_back();
    }

    /**
     * 从栈中移除并获取最后指针，不恢复指针
     */
    size_t TokenReader::getAndPopLastIndex() {
        size_t size = indexStack.size();
        if (size == 0) {
            return 0;
        }
        size_t result = indexStack[size - 1];
        pop();
        return result;
    }


    /**
     * 从栈中移除指针，恢复指针
     */
    void TokenReader::restore() {
        index = getAndPopLastIndex();
    }

    /**
     * 收集栈中最后一个指针位置到当前指针的token，从栈中移除指针，不恢复指针
     */
    VectorView <Token> CHelper::TokenReader::collect() {
        return {tokenList, getAndPopLastIndex(), index};
    }


} // CHelper