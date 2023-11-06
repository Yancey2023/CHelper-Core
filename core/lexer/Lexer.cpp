//
// Created by Yancey666 on 2023/11/6.
//

#include <algorithm>
#include "Lexer.h"

char endChars[] = " ,@~^/$&\"'!#%+*=[{]}\\|<>`\n";
char symbols[] = ",@~^/$&'!#%+*=[{]}\\|<>`+-=:\n";

[[maybe_unused]] Lexer::Lexer(std::string content, std::string filename) : stringReader(
        StringReader(std::move(content), std::move(filename))) {}

[[maybe_unused]] Lexer::Lexer(StringReader stringReader) : stringReader(std::move(stringReader)) {}

std::vector<Token> Lexer::lex() {
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
            case NUMBER:
                tokenList.push_back(nextTokenNumber(whiteSpace));
                break;
            case SYMBOL:
                tokenList.push_back(nextTokenSymbol(whiteSpace));
                break;
            case STRING:
                tokenList.push_back(nextTokenString(whiteSpace));
                break;
        }
    }
    return tokenList;
}

TokenType Lexer::nextTokenType() {
    char ch = stringReader.peek();
    if (isNum(ch)) {
        return NUMBER;
    } else if (ch == '+' || ch == '-') {
        stringReader.mark();
        ch = stringReader.next();
        stringReader.reset();
        if (isNum(ch)) {
            return NUMBER;
        } else {
            return SYMBOL;
        }
    } else if (isSymbol(ch)) {
        return SYMBOL;
    } else {
        return STRING;
    }
}

Token Lexer::nextTokenNumber(bool whiteSpace) {
    stringReader.mark();
    char ch = stringReader.peek();
    if (ch == '+' || ch == '-') {
        ch = stringReader.next();
    }
    while (isNum(ch)) {
        ch = stringReader.next();
    }
    return {NUMBER, whiteSpace, stringReader.posBackup, stringReader.collect()};
}

Token Lexer::nextTokenSymbol(bool whiteSpace) {
    char ch = stringReader.peek();
    std::string str = std::string();
    str.push_back(ch);
    Token result = {SYMBOL, whiteSpace, stringReader.pos, str};
    stringReader.skip();
    return result;
}

Token Lexer::nextTokenString(bool whiteSpace) {
    stringReader.mark();
    char ch = stringReader.peek();
    if(ch == '"'){
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
    return {STRING, whiteSpace, stringReader.posBackup, stringReader.collect()};
}

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