//
// Created by Yancey666 on 2023/11/6.
//

#include "Pos.h"
#include <iostream>


CHelper::Pos::Pos(int line,
                  int col,
                  int which,
                  std::string filePath)
        : line(line),
          col(col),
          which(which),
          filePath(std::move(filePath)) {}

void CHelper::Pos::next(char ch) {
    which++;
    if (ch == '\n') {
        line++;
        col = 1;
    } else {
        col++;
    }
}

std::ostream &CHelper::operator<<(std::ostream &os, const CHelper::Pos &pos) {
    return os << '('
              << pos.line
              << ',' << pos.col
              << ')';
}