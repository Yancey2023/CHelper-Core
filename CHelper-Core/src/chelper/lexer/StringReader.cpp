//
// Created by Yancey on 2023/11/6.
//

#include <chelper/lexer/StringReader.h>

namespace CHelper {

    CHelper::StringReader::StringReader(const std::u16string &content)
        : content(content),
          posBackup(pos) {}

    bool CHelper::StringReader::ready() const {
        return pos.index < content.length();
    }

    std::optional<char16_t> CHelper::StringReader::read() {
        if (HEDLEY_UNLIKELY(!ready())) {
            return std::nullopt;
        }
        char16_t ch = content[pos.index];
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

    std::optional<char16_t> CHelper::StringReader::next() {
        skip();
        return peek();
    }

    std::optional<char16_t> CHelper::StringReader::peek() const {
        if (HEDLEY_UNLIKELY(!ready())) {
            return std::nullopt;
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

    std::u16string_view CHelper::StringReader::collect() const {
        return {content.c_str() + posBackup.index, pos.index - posBackup.index};
    }

}// namespace CHelper