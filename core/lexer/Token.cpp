//
// Created by Yancey666 on 2023/11/6.
//

#include "Token.h"
#include <iostream>

Token::Token(TokenType type, bool whiteSpace, Pos pos, std::string content) : type(type),
                                                                              whiteSpace(whiteSpace),
                                                                              pos(std::move(pos)),
                                                                              content(std::move(content)) {}

std::ostream &operator<<(std::ostream &os, const TokenType &tokenType) {
    switch (tokenType) {
        case NUMBER:
            os << "数字";
            break;
        case STRING:
            os << "文字";
            break;
        case SYMBOL:
            os << "符号";
            break;
        default:
            os << "未知";
            break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
    return os << '[' << token.type << "] "<< token.pos << " (前面" << (token.whiteSpace ? "有" : "没有") << "空格) " << token.content << "";
}