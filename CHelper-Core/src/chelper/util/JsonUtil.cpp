//
// Created by Yancey on 2023/11/8.
//

#include <chelper/lexer/StringReader.h>
#include <chelper/parser/ErrorReason.h>
#include <chelper/util/JsonUtil.h>

namespace CHelper::JsonUtil {

    size_t ConvertResult::convert(size_t index) const {
        return indexConvertList[index];
    }

    std::u16string string2jsonString(const std::u16string &input) {
        std::u16string result;
        result.reserve(input.length());
        StringReader stringReader(input);
        while (true) {
            std::optional<char16_t> ch = stringReader.read();
            if (HEDLEY_UNLIKELY(!ch.has_value())) {
                return std::move(result);
            }
            switch (ch.value()) {
                case u'\"':
                case u'\\':
                case u'/':
                case u'\b':
                case u'\f':
                case u'\n':
                case u'\r':
                case u'\t':
                    result.push_back(u'\\');
                    break;
                default:
                    break;
            }
            result.push_back(ch.value());
        }
    }

    ConvertResult jsonString2String(const std::u16string &input) {
        ConvertResult result;
        if (HEDLEY_UNLIKELY(input.empty())) {
            result.errorReason = ErrorReason::incomplete(0, 0, u"json字符串必须在双引号内");
            return std::move(result);
        }
        StringReader stringReader(input);
        result.indexConvertList.push_back(stringReader.pos.index);
        int32_t unicodeValue;
        std::u16string escapeSequence;
        while (true) {
            std::optional<char16_t> ch = stringReader.next();
            //结束字符
            if (HEDLEY_UNLIKELY(!ch.has_value())) {
                result.isComplete = false;
                break;
            } else if (HEDLEY_UNLIKELY(ch.value() == u'\"')) {
                result.isComplete = true;
                break;
            }
            //正常字符
            if (HEDLEY_LIKELY(ch.value() != u'\\')) {
                result.result.push_back(ch.value());
                result.indexConvertList.push_back(stringReader.pos.index);
                continue;
            }
            //转义字符
            ch = stringReader.next();
            if (HEDLEY_UNLIKELY(!ch.has_value())) {
                result.errorReason = ErrorReason::incomplete(
                        stringReader.pos.index - 1,
                        stringReader.pos.index,
                        u"转义字符缺失后半部分");
            } else {
                switch (ch.value()) {
                    case u'\"':
                    case u'\\':
                    case u'/':
                    case u'b':
                    case u'f':
                    case u'n':
                    case u'r':
                    case u't':
                        result.result.push_back(ch.value());
                        result.indexConvertList.push_back(stringReader.pos.index);
                        break;
                    case u'u':
                        for (uint8_t i = 0; i < 4; ++i) {
                            ch = stringReader.next();
                            if (HEDLEY_UNLIKELY(!ch.has_value())) {
                                result.errorReason = ErrorReason::contentError(
                                        stringReader.pos.index - 2 - i,
                                        stringReader.pos.index,
                                        fmt::format(u"字符串转义缺失后半部分 -> \\u{}", escapeSequence));
                                break;
                            }
                            escapeSequence.push_back(ch.value());
                        }
                        if (HEDLEY_UNLIKELY(escapeSequence.length() < 4)) {
                            break;
                        }
                        if (HEDLEY_UNLIKELY(std::any_of(
                                    escapeSequence.begin(), escapeSequence.end(),
                                    [&result, &stringReader, &escapeSequence](const auto &item) {
                                        if (HEDLEY_LIKELY(std::isxdigit(item))) {
                                            return false;
                                        } else {
                                            result.errorReason = ErrorReason::incomplete(
                                                    stringReader.pos.index - escapeSequence.length() - 1,
                                                    stringReader.pos.index + 1,
                                                    fmt::format(u"字符串转义出现非法字符{} -> \\u{}", item, escapeSequence));
                                            return true;
                                        }
                                    }))) {
                            break;
                        }
                        unicodeValue = std::stoi(utf8::utf16to8(escapeSequence), nullptr, 16);
                        if (HEDLEY_UNLIKELY(unicodeValue <= 0 || unicodeValue > 0x10FFFF)) {
                            result.errorReason = ErrorReason::contentError(
                                    stringReader.pos.index - escapeSequence.length() - 1, stringReader.pos.index + 1,
                                    fmt::format(u"字符串转义的Unicode值无效 -> \\u{}", escapeSequence));
                            break;
                        }
                        escapeSequence.clear();
                        if (HEDLEY_LIKELY(unicodeValue <= 0x7F)) {
                            result.result.push_back(static_cast<char>(unicodeValue));
                        } else if (HEDLEY_LIKELY(unicodeValue <= 0x7FF)) {
                            result.result.push_back(
                                    static_cast<char>(0xC0u | (static_cast<uint32_t>(unicodeValue) >> 6u)));
                            result.result.push_back(
                                    static_cast<char>(0x80u | (static_cast<uint32_t>(unicodeValue) & 0x3Fu)));
                        } else if (HEDLEY_LIKELY(unicodeValue <= 0xFFFF)) {
                            result.result.push_back(
                                    static_cast<char>(0xE0u | (static_cast<uint32_t>(unicodeValue) >> 12u)));
                            result.result.push_back(
                                    static_cast<char>(0x80u | ((static_cast<uint32_t>(unicodeValue) >> 6u) & 0x3Fu)));
                            result.result.push_back(
                                    static_cast<char>(0x80u | (static_cast<uint32_t>(unicodeValue) & 0x3Fu)));
                        } else {
                            result.result.push_back(
                                    static_cast<char>(0xF0u | (static_cast<uint32_t>(unicodeValue) >> 18u)));
                            result.result.push_back(
                                    static_cast<char>(0x80u | ((static_cast<uint32_t>(unicodeValue) >> 12u) & 0x3Fu)));
                            result.result.push_back(
                                    static_cast<char>(0x80u | ((static_cast<uint32_t>(unicodeValue) >> 6u) & 0x3Fu)));
                            result.result.push_back(
                                    static_cast<char>(0x80u | (static_cast<uint32_t>(unicodeValue) & 0x3Fu)));
                        }
                        result.indexConvertList.push_back(stringReader.pos.index);
                        break;
                    default:
                        result.errorReason = ErrorReason::contentError(
                                stringReader.pos.index - 1, stringReader.pos.index + 1,
                                fmt::format(u"未知的转义字符 -> \\{:c}", ch.value()));
                        break;
                }
            }
            if (HEDLEY_UNLIKELY(result.errorReason != nullptr)) {
                break;
            }
        }
        return std::move(result);
    }

}// namespace CHelper::JsonUtil