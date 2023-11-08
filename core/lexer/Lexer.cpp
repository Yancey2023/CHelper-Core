//
// Created by Yancey666 on 2023/11/6.
//

#include <algorithm>
#include "Lexer.h"

char endChars[] = " ,@~^/$&\"'!#%+*=[{]}\\|<>`\n";
char symbols[] = ",@~^/$&'!#%+*=[{]}\\|<>`+-=:\n";

bool isNum(char ch) {
    return (ch >= '0' && ch <= '9') || ch == '.';
}

bool isEndChar(char ch) {
    for (const auto &endChar: endChars) {
        if (ch == endChar) {
            return true;
        }
    }
    return false;
}

bool isSymbol(char ch) {
    for (const auto &ch_: symbols) {
        if (ch == ch_) {
            return true;
        }
    }
    return false;
}

[[maybe_unused]] CHelper::Lexer::Lexer(std::string content,
                                       std::string filename)
        : stringReader(StringReader(std::move(content), std::move(filename))) {}

[[maybe_unused]] CHelper::Lexer::Lexer(StringReader stringReader)
        : stringReader(std::move(stringReader)) {}

std::vector<CHelper::Token> CHelper::Lexer::lex() {
    std::vector<Token> tokenList = std::vector<Token>();
    char ch;
    bool whiteSpace;
    while (true) {
        ch = stringReader.peek();
        if (ch == -1) {
            break;
        }
        whiteSpace = false;
        while (ch == ' ') {
            ch = stringReader.next();
            whiteSpace = true;
        }
        if (ch == -1) {
            break;
        }
        switch (nextTokenType()) {
            case CHelper::TokenType::NUMBER:
                tokenList.push_back(nextTokenNumber(whiteSpace));
                break;
            case CHelper::TokenType::SYMBOL:
                tokenList.push_back(nextTokenSymbol(whiteSpace));
                break;
            case CHelper::TokenType::STRING:
                tokenList.push_back(nextTokenString(whiteSpace));
                break;
        }
    }
    return tokenList;
}

CHelper::TokenType::TokenType CHelper::Lexer::nextTokenType() {
    char ch = stringReader.peek();
    if (isNum(ch)) {
        return CHelper::TokenType::NUMBER;
    } else if (ch == '+' || ch == '-') {
        stringReader.mark();
        ch = stringReader.next();
        stringReader.reset();
        if (isNum(ch)) {
            return CHelper::TokenType::NUMBER;
        } else {
            return CHelper::TokenType::SYMBOL;
        }
    } else if (isSymbol(ch)) {
        return CHelper::TokenType::SYMBOL;
    } else {
        return CHelper::TokenType::STRING;
    }
}

CHelper::Token CHelper::Lexer::nextTokenNumber(bool whiteSpace) {
    stringReader.mark();
    char ch = stringReader.peek();
    if (ch == '+' || ch == '-') {
        ch = stringReader.next();
    }
    while (isNum(ch)) {
        ch = stringReader.next();
    }
    return {CHelper::TokenType::NUMBER, whiteSpace, stringReader.posBackup, stringReader.collect()};
}

CHelper::Token CHelper::Lexer::nextTokenSymbol(bool whiteSpace) {
    char ch = stringReader.peek();
    std::string str = std::string();
    str.push_back(ch);
    Token result = {CHelper::TokenType::SYMBOL, whiteSpace, stringReader.pos, str};
    stringReader.skip();
    return result;
}

CHelper::Token CHelper::Lexer::nextTokenString(bool whiteSpace) {
    stringReader.mark();
    char ch = stringReader.peek();
    if (ch == '"') {
        ch = ch;
    }
    bool containSpace = ch == '"';
    if (containSpace) {
        ch = stringReader.next();
    }
    while (true) {
        if (ch == -1) {
            break;
        }
        if (containSpace) {
            if (ch == '"') {
                stringReader.skip();
                break;
            }
        } else if (isEndChar(ch)) {
            break;
        }
        ch = stringReader.next();
    }
    return {CHelper::TokenType::STRING, whiteSpace, stringReader.posBackup, stringReader.collect()};
}