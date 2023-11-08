//
// Created by Yancey666 on 2023/11/6.
//

#include "StringReader.h"

CHelper::StringReader::StringReader(
        std::string content,
        std::string filePath)
        : content(std::move(content)),
          pos(CHelper::Pos(1, 0, 0, std::move(filePath))),
          posBackup(pos) {}

bool CHelper::StringReader::ready() const {
    return pos.which < content.length();
}

char CHelper::StringReader::read() {
    if (!ready()) {
        return -1;
    }
    char ch = content[pos.which];
    pos.next(ch);
    return ch;
}

bool CHelper::StringReader::skip() {
    if (!ready()) {
        return false;
    }
    pos.next(content[pos.which]);
    return true;
}

char CHelper::StringReader::next() {
    skip();
    return peek();
}

char CHelper::StringReader::peek() {
    if (!ready()) {
        return -1;
    }
    return content[pos.which];
}

void CHelper::StringReader::mark() {
    posBackup.line = pos.line;
    posBackup.col = pos.col;
    posBackup.which = pos.which;
}

void CHelper::StringReader::reset() {
    pos.line = posBackup.line;
    pos.col = posBackup.col;
    pos.which = posBackup.which;
}

std::string CHelper::StringReader::collect() const {
    return {content, (size_t) posBackup.which, (size_t)(pos.which - posBackup.which)};
}
