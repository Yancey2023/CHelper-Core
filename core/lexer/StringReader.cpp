//
// Created by Yancey666 on 2023/11/6.
//

#include "StringReader.h"

StringReader::StringReader(std::string content, std::string filePath) : content(std::move(content)),
                                                                        pos(Pos(1, 0, 0, std::move(filePath))),
                                                                        posBackup(pos) {}

bool StringReader::ready() const {
    return pos.which < content.length();
}

char StringReader::read() {
    if (!ready()) {
        return -1;
    }
    char ch = content[pos.which];
    pos.next(ch);
    return ch;
}

bool StringReader::skip() {
    if (!ready()) {
        return false;
    }
    pos.next(content[pos.which]);
    return true;
}

char StringReader::next() {
    skip();
    return peek();
}

char StringReader::peek() {
    if (!ready()) {
        return -1;
    }
    return content[pos.which];
}

void StringReader::mark() {
    posBackup.line = pos.line;
    posBackup.col = pos.col;
    posBackup.which = pos.which;
}

void StringReader::reset() {
    pos.line = posBackup.line;
    pos.col = posBackup.col;
    pos.which = posBackup.which;
}

std::string StringReader::collect() const {
    return {content, (size_t) posBackup.which, (size_t) (pos.which - posBackup.which)};
}
