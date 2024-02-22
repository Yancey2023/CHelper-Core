//
// Created by Yancey666 on 2023/11/6.
//

#include "StringReader.h"

namespace CHelper {

    CHelper::StringReader::StringReader(
            const std::string &content,
            const std::string &filePath)
            : content(content),
              pos(LexerPos(1, 0, 0, filePath)),
              posBackup(pos) {}

    bool CHelper::StringReader::ready() const {
        return pos.index < content.length();
    }

    char CHelper::StringReader::read() {
        if (!ready()) {
            return EOF;
        }
        char ch = content[pos.index];
        pos.next(ch);
        return ch;
    }

    bool CHelper::StringReader::skip() {
        if (!ready()) {
            return false;
        }
        pos.next(content[pos.index]);
        return true;
    }

    char CHelper::StringReader::next() {
        skip();
        return peek();
    }

    char CHelper::StringReader::peek() const {
        if (!ready()) {
            return EOF;
        }
        return content[pos.index];
    }

    void CHelper::StringReader::mark() {
        posBackup.line = pos.line;
        posBackup.col = pos.col;
        posBackup.index = pos.index;
    }

    void CHelper::StringReader::reset() {
        pos.line = posBackup.line;
        pos.col = posBackup.col;
        pos.index = posBackup.index;
    }

    std::string CHelper::StringReader::collect() const {
        return {content, (size_t) posBackup.index, (size_t) (pos.index - posBackup.index)};
    }

}