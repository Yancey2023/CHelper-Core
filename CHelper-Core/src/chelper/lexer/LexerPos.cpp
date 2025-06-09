//
// Created by Yancey on 2023/11/6.
//

#include <chelper/lexer/LexerPos.h>

CHelper::LexerPos::LexerPos(size_t index,
                            size_t line,
                            size_t col)
    : index(index),
      line(line),
      col(col) {}

void CHelper::LexerPos::next(char16_t ch) {
    index++;
    if (HEDLEY_UNLIKELY(ch == '\n')) {
        line++;
        col = 1;
    } else {
        col++;
    }
}