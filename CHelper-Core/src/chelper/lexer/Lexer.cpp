//
// Created by Yancey on 2023/11/6.
//

#include <chelper/lexer/Lexer.h>

namespace CHelper::Lexer {

    //字符串的结束字符
    const std::u16string endChars = u",@~^/$&'!#%+*=[{]}\\|<>`\" \n";
    //可以被识别成符号的字符，这些字符不一定是字符串的结束字符
    const std::u16string symbols = u",@~^/$&'!#%+*=[{]}\\|<>`-:";

    bool isNumberChar(char16_t ch) {
        return (ch >= '0' && ch <= '9') || ch == '.';
    }

    bool isStringEndChar(char16_t ch) {
        return std::any_of(endChars.begin(), endChars.end(), [&ch](const char16_t &endChar) {
            return ch == endChar;
        });
    }

    bool isSymbolChar(char16_t ch) {
        return std::any_of(symbols.begin(), symbols.end(), [&ch](const char16_t &endChar) {
            return ch == endChar;
        });
    }

    std::optional<TokenType::TokenType> getNextTokenType(StringReader &stringReader) {
        std::optional<char16_t> ch = stringReader.peek();
        if (!HEDLEY_UNLIKELY(ch.has_value())) {
            return std::nullopt;
        } else if (HEDLEY_UNLIKELY(ch.value() == '\n')) {
            return TokenType::LF;
        } else if (HEDLEY_UNLIKELY(ch.value() == ' ')) {
            return TokenType::SPACE;
        } else if (HEDLEY_UNLIKELY(isNumberChar(ch.value()))) {
            return TokenType::NUMBER;
        } else if (HEDLEY_UNLIKELY(ch.value() == '+' || ch.value() == '-')) {
            stringReader.mark();
            ch = stringReader.next();
            stringReader.reset();
            if (HEDLEY_LIKELY(ch.has_value() && isNumberChar(ch.value()))) {
                return TokenType::NUMBER;
            } else {
                return TokenType::SYMBOL;
            }
        } else if (HEDLEY_UNLIKELY(isSymbolChar(ch.value()))) {
            return TokenType::SYMBOL;
        } else {
            return TokenType::STRING;
        }
    }

    Token nextTokenNumber(StringReader &stringReader) {
        stringReader.mark();
        std::optional<char16_t> ch = stringReader.peek();
        while (ch.has_value() && (isNumberChar(ch.value()) || ch.value() == '+' || ch.value() == '-')) {
            ch = stringReader.next();
        }
        return {TokenType::NUMBER, stringReader.posBackup, stringReader.collect()};
    }

    Token nextTokenSymbol(StringReader &stringReader) {
        stringReader.mark();
        stringReader.skip();
        return {TokenType::SYMBOL, stringReader.posBackup, stringReader.collect()};
    }

    Token nextTokenString(StringReader &stringReader) {
        // NOTE: please ensure stringReader.peek().has_value() before call nextTokenString()
        stringReader.mark();
        std::optional<char16_t> ch = stringReader.peek();
        bool isDoubleQuotation = ch.value() == '"';
        if (HEDLEY_UNLIKELY(isDoubleQuotation)) {
            ch = stringReader.next();
        }
        while (true) {
            if (HEDLEY_UNLIKELY(!ch.has_value())) {
                break;
            }
            if (HEDLEY_UNLIKELY(ch.value() == '\\')) {
                //转义字符
                ch = stringReader.next();
                if (HEDLEY_UNLIKELY(!ch.has_value())) {
                    break;
                }
            } else if (HEDLEY_UNLIKELY(isDoubleQuotation)) {
                //如果是双引号开头，只能使用双引号结尾
                if (HEDLEY_UNLIKELY(ch.value() == '"')) {
                    stringReader.skip();
                    break;
                }
            } else if (HEDLEY_UNLIKELY(isStringEndChar(ch.value()))) {
                //在检测到字符串结束字符时进行结尾
                break;
            }
            ch = stringReader.next();
        }
        return {TokenType::STRING, stringReader.posBackup, stringReader.collect()};
    }

    Token nextTokenSpace(StringReader &stringReader) {
        stringReader.mark();
        stringReader.skip();
        return {TokenType::SPACE, stringReader.posBackup, stringReader.collect()};
    }

    Token nextTokenLF(StringReader &stringReader) {
        stringReader.mark();
        stringReader.skip();
        return {TokenType::LF, stringReader.posBackup, stringReader.collect()};
    }

    LexerResult lex(const std::u16string &content) {
        StringReader stringReader(content);
#ifdef CHelperTest
        Profile::push("start lex: {}", FORMAT_ARG(utf8::utf16to8(stringReader.content)));
#endif
        std::vector<Token> tokenList = std::vector<Token>();
        while (true) {
            std::optional<TokenType::TokenType> nextTokenType = getNextTokenType(stringReader);
            if (HEDLEY_UNLIKELY(!nextTokenType.has_value())) {
                break;
            }
            switch (nextTokenType.value()) {
                case TokenType::NUMBER:
                    tokenList.push_back(nextTokenNumber(stringReader));
                    break;
                case TokenType::SYMBOL:
                    tokenList.push_back(nextTokenSymbol(stringReader));
                    break;
                case TokenType::STRING:
                    tokenList.push_back(nextTokenString(stringReader));
                    break;
                case TokenType::SPACE:
                    tokenList.push_back(nextTokenSpace(stringReader));
                    break;
                case TokenType::LF:
                    tokenList.push_back(nextTokenLF(stringReader));
                    break;
            }
        }
#ifdef CHelperTest
        Profile::pop();
#endif
        return {content, std::move(tokenList)};
    }

}// namespace CHelper::Lexer