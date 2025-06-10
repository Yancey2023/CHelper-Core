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
                case SPACE:
                    return u"空格";
                case LF:
                    return u"换行符类型";
                default:
                    return u"未知类型";
            }
        }

    }// namespace TokenType

    Token::Token(TokenType::TokenType type,
                 const CHelper::LexerPos &pos,
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