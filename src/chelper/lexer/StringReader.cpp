//
// Created by Yancey on 2023/11/6.
//

#include "StringReader.h"

namespace CHelper {

    CHelper::StringReader::StringReader(const std::string &content)
        : content(content),
          posBackup(pos) {}

    bool CHelper::StringReader::ready() const {
        return pos.index < content.length();
    }

    signed char CHelper::StringReader::read() {
        if (HEDLEY_UNLIKELY(!ready())) {
            return EOF;
        }
        char ch = content[pos.index];
        pos.next(ch);
        return ch;
    }

    bool CHelper::StringReader::skip() {
        if (HEDLEY_UNLIKELY(!ready())) {
            return false;
        }
        pos.next(content[pos.index]);
        return true;
    }

    signed char CHelper::StringReader::next() {
        skip();
        return peek();
    }

    signed char CHelper::StringReader::peek() const {
        if (HEDLEY_UNLIKELY(!ready())) {
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

    std::string_view CHelper::StringReader::collect() const {
        return {content.c_str() + posBackup.index, pos.index - posBackup.index};
    }

}// namespace CHelper