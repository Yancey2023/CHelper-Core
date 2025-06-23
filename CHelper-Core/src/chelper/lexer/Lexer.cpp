//
// Created by Yancey on 2023/11/6.
//

#include <chelper/lexer/Lexer.h>

namespace CHelper::Lexer {

    class Lexer {
    private:
        const std::u16string &content;
        size_t index = 0;
        std::vector<Token> tokens;

    public:
        explicit Lexer(const std::u16string &content)
            : content(content) {}

    private:
        void getNumberToken(size_t startIndex) {
            while (++index < content.size()) {
                switch (content[index]) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                    case '.':
                    case '+':
                    case '-':
                        break;
                    default:
                        tokens.emplace_back(TokenType::NUMBER, startIndex, std::u16string_view(content.data() + startIndex, index - startIndex));
                        return;
                }
            }
            tokens.emplace_back(TokenType::NUMBER, startIndex, std::u16string_view(content.data() + startIndex, content.size() - startIndex));
        }

        void getStringToken(bool isDoubleQuote) {
            size_t startIndex = index;
            while (++index < content.size()) {
                char16_t ch = content[index];
                if (ch == '\\') {
                    ++index;
                } else if (isDoubleQuote) {
                    if (ch == '"') {
                        ++index;
                        tokens.emplace_back(TokenType::STRING, startIndex, std::u16string_view(content.data() + startIndex, index - startIndex));
                        return;
                    }
                } else {
                    switch (ch) {
                        case u',':
                        case u'@':
                        case u'~':
                        case u'^':
                        case u'/':
                        case u'$':
                        case u'&':
                        case u'\'':
                        case u'!':
                        case u'#':
                        case u'%':
                        case u'+':
                        case u'*':
                        case u'=':
                        case u'[':
                        case u'{':
                        case u']':
                        case u'}':
                        case u'\\':
                        case u'|':
                        case u'<':
                        case u'>':
                        case u'`':
                        case u'\"':
                        case u' ':
                        case u'\n':
                            tokens.emplace_back(TokenType::STRING, startIndex, std::u16string_view(content.data() + startIndex, index - startIndex));
                            return;
                        default:
                            break;
                    }
                }
            }
            tokens.emplace_back(TokenType::STRING, startIndex, std::u16string_view(content.data() + startIndex, content.size() - startIndex));
        }

    public:
        void run() {
            while (index < content.size()) {
                char16_t ch = content[index];
                switch (ch) {
                    case '\n': {
                        tokens.emplace_back(TokenType::LF, index, std::u16string_view(content.data() + index, 1));
                        ++index;
                        break;
                    }
                    case ' ': {
                        tokens.emplace_back(TokenType::SPACE, index, std::u16string_view(content.data() + index, 1));
                        ++index;
                        break;
                    }
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                    case '.': {
                        getNumberToken(index);
                        break;
                    }
                    case '+':
                    case '-': {
                        size_t startIndex = index;
                        bool isNumber = false;
                        if (++index < content.size()) {
                            switch (content[index]) {
                                case '0':
                                case '1':
                                case '2':
                                case '3':
                                case '4':
                                case '5':
                                case '6':
                                case '7':
                                case '8':
                                case '9':
                                case '.':
                                    isNumber = true;
                                    break;
                                default:
                                    break;
                            }
                        }
                        if (isNumber) {
                            getNumberToken(startIndex);
                        } else {
                            tokens.emplace_back(TokenType::SYMBOL, startIndex, std::u16string_view(content.data() + startIndex, 1));
                        }
                        break;
                    }
                    case u',':
                    case u'@':
                    case u'~':
                    case u'^':
                    case u'/':
                    case u'$':
                    case u'&':
                    case u'\'':
                    case u'!':
                    case u'#':
                    case u'%':
                    case u'*':
                    case u'=':
                    case u'[':
                    case u'{':
                    case u']':
                    case u'}':
                    case u'\\':
                    case u'|':
                    case u'<':
                    case u'>':
                    case u'`':
                    case u':': {
                        tokens.emplace_back(TokenType::SYMBOL, index, std::u16string_view(content.data() + index, 1));
                        ++index;
                        break;
                    }
                    case '\"':
                        getStringToken(true);
                        break;
                    default: {
                        getStringToken(false);
                        break;
                    }
                }
            }
        }

        std::vector<Token> getResult() {
            return std::move(tokens);
        }
    };

    LexerResult lex(const std::u16string &content) {
#ifdef CHelperTest
        Profile::push("start lex: {}", FORMAT_ARG(utf8::utf16to8(content)));
#endif
        Lexer lexer(content);
        lexer.run();
#ifdef CHelperTest
        Profile::pop();
#endif
        return {content, lexer.getResult()};
    }

}// namespace CHelper::Lexer