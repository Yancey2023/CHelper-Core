//
// Created by Yancey666 on 2023/11/6.
//

#include "Token.h"

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
            os << "NUMBER";
            break;
        case CHelper::TokenType::STRING:
            os << "STRING";
            break;
        case CHelper::TokenType::SYMBOL:
            os << "SYMBOL";
            break;
        default:
            os << "UNKNOWN";
            break;
    }
    return os;
}

std::ostream &CHelper::operator<<(std::ostream &os, const CHelper::Token &token) {
    os << '['
       << token.type
       << "] "
       << token.pos;
    if (token.whiteSpace) {
        os << "after whitespace";
    }
    return os << token.content;
}