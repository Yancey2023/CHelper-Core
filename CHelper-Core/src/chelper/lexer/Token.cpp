//
// Created by Yancey on 2023/11/6.
//

#include <chelper/lexer/Token.h>

namespace CHelper {

    namespace TokenType {

        std::u16string getName(TokenType tokenType) {
            switch (tokenType) {
                case STRING:
                    return u"字符串类型";
                case NUMBER:
                    return u"数字类型";
                case SYMBOL:
                    return u"符号类型";
                case WHITE_SPACE:
                    return u"空格";
                case LF:
                    return u"换行符类型";
                default:
                    return u"未知类型";
            }
        }

    }// namespace TokenType

    Token::Token(TokenType::TokenType type,
                 CHelper::LexerPos pos,
                 const std::u16string_view &content)
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

#ifdef CHelperTest
std::ostream &operator<<(std::ostream &os, const CHelper::TokenType::TokenType &tokenType) {
    switch (tokenType) {
        case CHelper::TokenType::NUMBER:
            os << "NUMBER";
            break;
        case CHelper::TokenType::STRING:
            os << "STRING";
            break;
        case CHelper::TokenType::SYMBOL:
            os << "SYMBOu";
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