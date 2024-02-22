//
// Created by Yancey666 on 2023/11/6.
//

#include "Token.h"

namespace CHelper {

    namespace TokenType {
        std::string getName(TokenType tokenType) {
            switch (tokenType) {
                case STRING:
                    return "字符串类型";
                case NUMBER:
                    return "数字类型";
                case SYMBOL:
                    return "符号类型";
                case LF:
                    return "换行符类型";
                default:
                    return "未知类型";
            }
        }
    }

    Token::Token(TokenType::TokenType type,
                 bool whiteSpace,
                 CHelper::LexerPos pos,
                 std::string content)
            : type(type),
              whiteSpace(whiteSpace),
              pos(pos),
              content(std::move(content)) {}

    size_t Token::getStartIndex() const {
        return pos.index;
    }

    size_t Token::getEndIndex() const {
        return pos.index + content.size();
    }

}

std::ostream &operator<<(std::ostream &os, const CHelper::TokenType::TokenType &tokenType) {
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

std::ostream &operator<<(std::ostream &os, const CHelper::Token &token) {
    os << '['
       << token.type
       << "] "
       << token.pos;
    if (token.whiteSpace) {
        os << " (after whitespace)";
    }
    return os << ' ' << token.content;
}