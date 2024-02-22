//
// Created by Yancey666 on 2023/12/15.
//

#include "TokenReader.h"

namespace CHelper {

    TokenReader::TokenReader(const std::vector<CHelper::Token> &tokenList)
            : tokenList(tokenList) {}

    bool TokenReader::ready() const {
        return index < tokenList.size();
    }

    const Token *TokenReader::read() {
        if (!ready()) {
            return nullptr;
        }
        return &tokenList[index++];
    }

    bool TokenReader::skip() {
        if (!ready()) {
            return false;
        }
        index++;
        return true;
    }

    void TokenReader::skipToLF() {
        while (ready()) {
            if (read()->type == TokenType::LF) {
                break;
            }
        }
    }

    const Token *TokenReader::next() {
        skip();
        return peek();
    }

    const Token *TokenReader::peek() const {
        if (!ready()) {
            return nullptr;
        }
        return &tokenList[index];
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
    VectorView<Token> CHelper::TokenReader::collect() {
        return {&tokenList, getAndPopLastIndex(), index};
    }


} // CHelper