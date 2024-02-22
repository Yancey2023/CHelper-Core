//
// Created by Yancey666 on 2023/11/6.
//

#include "LexerPos.h"

CHelper::LexerPos::LexerPos(size_t line,
                            size_t col,
                            size_t which,
                            const std::string &filePath)
        : line(line),
          col(col),
          index(which),
          filePath(filePath) {}

void CHelper::LexerPos::next(char ch) {
    index++;
    if (ch == '\n') {
        line++;
        col = 1;
    } else {
        col++;
    }
}

std::ostream &CHelper::operator<<(std::ostream &os, const CHelper::LexerPos &pos) {
    return os << '('
              << pos.line
              << ','
              << pos.col
              << ')';
}