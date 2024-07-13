//
// Created by Yancey on 2023/11/6.
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
                case WHITE_SPACE:
                    return "空格";
                case LF:
                    return "换行符类型";
                default:
                    return "未知类型";
            }
        }

        CODEC_ENUM(TokenType, uint8_t)

    }// namespace TokenType

    Token::Token(TokenType::TokenType type,
                 CHelper::LexerPos pos,
                 const std::string_view &content)
        : type(type),
          pos(pos),
          content(content) {}

    size_t Token::getStartIndex() const {
        return pos.index;
    }

    size_t Token::getEndIndex() const {
        return pos.index + content.size();
    }

}// namespace CHelper

#if CHelperTest == true
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
        case CHelper::TokenType::WHITE_SPACE:
            os << "WHITE_SPACE";
            break;
        default:
            os << "UNKNOWN";
            break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const CHelper::Token &token) {
    return os << '['
              << token.type
              << "] "
              << token.pos
              << ' '
              << token.content;
}
#endif