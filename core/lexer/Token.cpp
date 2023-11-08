//
// Created by Yancey666 on 2023/11/6.
//

#include "Token.h"
#include <iostream>

CHelper::Token::Token(TokenType::TokenType type,
                      bool whiteSpace,
                      CHelper::Pos pos,
                      std::string content)
        : type(type),
          whiteSpace(whiteSpace),
          pos(std::move(pos)),
          content(std::move(content)) {}

std::ostream &CHelper::operator<<(std::ostream &os, const CHelper::TokenType::TokenType &tokenType) {
    switch (tokenType) {
        case CHelper::TokenType::NUMBER:
            os << "数字";
            break;
        case CHelper::TokenType::STRING:
            os << "文字";
            break;
        case CHelper::TokenType::SYMBOL:
            os << "符号";
            break;
        default:
            os << "未知";
            break;
    }
    return os;
}

std::ostream &CHelper::operator<<(std::ostream &os, const CHelper::Token &token) {
    return os << '['
              << token.type
              << "] "
              << token.pos
              << " (前面"
              << (token.whiteSpace ? "有" : "没有")
              << "空格) "
              << token.content
              << "";
}