//
// Created by Yancey on 2023/11/6.
//

#include "LexerPos.h"

CHelper::LexerPos::LexerPos(size_t index,
                            size_t line,
                            size_t col)
    : line(line),
      col(col),
      index(index) {}

void CHelper::LexerPos::next(wchar_t ch) {
    index++;
    if (HEDLEY_UNLIKELY(ch == '\n')) {
        line++;
        col = 1;
    } else {
        col++;
    }
}

std::ostream &CHelper::operator<<(std::ostream &os, const CHelper::LexerPos &pos) {
    return os << '(' << pos.line << ',' << pos.col << ')';
}