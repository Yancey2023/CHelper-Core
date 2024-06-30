//
// Created by Yancey on 2023/11/6.
//

#include "Lexer.h"

namespace CHelper::Lexer {

    //字符串的结束字符
    const std::string endChars = " ,@~^/$&\"'!#%+*=[{]}\\|<>`\n";
    //可以被识别成符号的字符，这些字符不一定是字符串的结束字符
    const std::string symbols = ",@~^/$&'!#%+*=[{]}\\|<>`+-=:";

    bool isNum(signed char ch) {
        return (ch >= '0' && ch <= '9') || ch == '.';
    }

    bool isEndChar(signed char ch) {
        return std::any_of(endChars.begin(), endChars.end(), [&ch](const char &endChar) {
            return ch == endChar;
        });
    }

    bool isSymbol(signed char ch) {
        return std::any_of(symbols.begin(), symbols.end(), [&ch](const char &endChar) {
            return ch == endChar;
        });
    }

    TokenType::TokenType nextTokenType(StringReader &stringReader) {
        char ch = stringReader.peek();
        if (HEDLEY_UNLIKELY(ch == '\n')) {
            return TokenType::LF;
        } else if (HEDLEY_LIKELY(ch == ' ')) {
            return TokenType::WHITE_SPACE;
        } else if (HEDLEY_UNLIKELY(isNum(ch))) {
            return TokenType::NUMBER;
        } else if (HEDLEY_UNLIKELY(ch == '+' || ch == '-')) {
            stringReader.mark();
            ch = stringReader.next();
            stringReader.reset();
            if (HEDLEY_LIKELY(isNum(ch))) {
                return TokenType::NUMBER;
            } else {
                return TokenType::SYMBOL;
            }
        } else if (HEDLEY_UNLIKELY(isSymbol(ch))) {
            return TokenType::SYMBOL;
        } else {
            return TokenType::STRING;
        }
    }

    Token nextTokenNumber(StringReader &stringReader) {
        stringReader.mark();
        signed char ch = stringReader.peek();
        while (isNum(ch) || ch == '+' || ch == '-') {
            ch = stringReader.next();
        }
        return {TokenType::NUMBER, stringReader.posBackup, stringReader.collect()};
    }

    Token nextTokenSymbol(StringReader &stringReader) {
        std::string str = std::string();
        str.push_back(stringReader.peek());
        Token result = {TokenType::SYMBOL, stringReader.pos, std::move(str)};
        stringReader.skip();
        return std::move(result);
    }

    Token nextTokenString(StringReader &stringReader) {
        stringReader.mark();
        signed char ch = stringReader.peek();
        bool isDoubleQuotation = ch == '"';
        if (HEDLEY_UNLIKELY(isDoubleQuotation)) {
            ch = stringReader.next();
        }
        while (true) {
            if (HEDLEY_UNLIKELY(ch == EOF)) {
                break;
            }
            if (HEDLEY_UNLIKELY(ch == '\\')) {
                //转义字符
                ch = stringReader.next();
                if (HEDLEY_UNLIKELY(ch == EOF)) {
                    break;
                }
            } else if (HEDLEY_UNLIKELY(isDoubleQuotation)) {
                //如果是双引号开头，只能使用双引号结尾
                if (HEDLEY_UNLIKELY(ch == '"')) {
                    stringReader.skip();
                    break;
                }
            } else if (HEDLEY_UNLIKELY(isEndChar(ch))) {
                //在检测到字符串结束字符时进行结尾
                break;
            }
            ch = stringReader.next();
        }
        return {TokenType::STRING, stringReader.posBackup, stringReader.collect()};
    }

    Token nextTokenWhiteSpace(StringReader &stringReader) {
        std::string str = std::string();
        str.push_back(stringReader.peek());
        Token result = {TokenType::WHITE_SPACE, stringReader.pos, std::move(str)};
        stringReader.skip();
        return std::move(result);
    }

    Token nextTokenLF(StringReader &stringReader) {
        std::string str = std::string();
        str.push_back(stringReader.peek());
        Token result = {TokenType::LF, stringReader.pos, std::move(str)};
        stringReader.skip();
        return std::move(result);
    }

    std::vector<Token> lex(StringReader stringReader) {
        Profile::push("start lex: " + stringReader.content);
        std::vector<Token> tokenList = std::vector<Token>();
        while (true) {
            if (HEDLEY_UNLIKELY(stringReader.peek() == EOF)) {
                break;
            }
            switch (nextTokenType(stringReader)) {
                case TokenType::NUMBER:
                    tokenList.push_back(nextTokenNumber(stringReader));
                    break;
                case TokenType::SYMBOL:
                    tokenList.push_back(nextTokenSymbol(stringReader));
                    break;
                case TokenType::STRING:
                    tokenList.push_back(nextTokenString(stringReader));
                    break;
                case TokenType::WHITE_SPACE:
                    tokenList.push_back(nextTokenWhiteSpace(stringReader));
                    break;
                case TokenType::LF:
                    tokenList.push_back(nextTokenLF(stringReader));
                    break;
            }
        }
        Profile::pop();
        return tokenList;
    }

}// namespace CHelper::Lexer