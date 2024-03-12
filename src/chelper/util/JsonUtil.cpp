//
// Created by Yancey on 2023/11/8.
//

#include "JsonUtil.h"
#include "../lexer/StringReader.h"
#include "../parser/ErrorReason.h"

namespace CHelper::JsonUtil {

    size_t ConvertResult::convert(size_t index) {
        return indexConvertList[index];
    }

    std::string string2jsonString(const std::string &input) {
        std::string result;
        StringReader stringReader(input, "unknown");
        while (true) {
            char ch = stringReader.peek();
            switch (ch) {
                case EOF:
                    return result;
                case '\"':
                case '\\':
                case '/':
                case '\b':
                case '\f':
                case '\n':
                case '\r':
                case '\t':
                    result.push_back('\\');
                    result.push_back(ch);
                    stringReader.skip();
                    break;
                default:
                    result.push_back(ch);
                    break;
            }
            stringReader.skip();
        }
    }

    ConvertResult jsonString2String(const std::string &input) {
        ConvertResult result;
        if (input.empty()) {
            result.errorReason = ErrorReason::incomplete(0, 0, "json字符串必须在双引号内");
            return result;
        }
        StringReader stringReader(input, "unknown");
        result.indexConvertList.push_back(stringReader.pos.index);
        int unicodeValue;
        std::string escapeSequence;
        while (true) {
            char ch = stringReader.next();
            //结束字符
            if (ch == EOF) {
                result.isComplete = false;
                break;
            } else if (ch == '\"') {
                result.isComplete = true;
                break;
            }
            //正常字符
            if (ch != '\\') {
                result.result.push_back(ch);
                result.indexConvertList.push_back(stringReader.pos.index);
                continue;
            }
            //转义字符
            ch = stringReader.next();
            switch (ch) {
                case EOF:
                    result.errorReason = ErrorReason::incomplete(stringReader.pos.index - 1, stringReader.pos.index,
                                                                 "转义字符缺失后半部分");
                    break;
                case '\"':
                case '\\':
                case '/':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                    result.result.push_back(ch);
                    result.indexConvertList.push_back(stringReader.pos.index);
                    break;
                case 'u':
                    for (int i = 0; i < 4; ++i) {
                        ch = stringReader.next();
                        if (ch == EOF) {
                            result.errorReason = ErrorReason::contentError(
                                    stringReader.pos.index - 2 - i,
                                    stringReader.pos.index,
                                    "字符串转义缺失后半部分 -> \\u" + escapeSequence);
                            break;
                        }
                        escapeSequence.push_back(ch);
                    }
                    if (escapeSequence.length() < 4) {
                        break;
                    }
                    if (std::any_of(escapeSequence.begin(), escapeSequence.end(),
                                    [&result, &stringReader, &escapeSequence](const auto &item) {
                                        bool notHex = !std::isxdigit(item);
                                        if (notHex) {
                                            result.errorReason = ErrorReason::incomplete(
                                                    stringReader.pos.index - escapeSequence.length() - 1,
                                                    stringReader.pos.index + 1,
                                                    FormatUtil::format(
                                                            "字符串转义出现非法字符{0} -> \\u{1}",
                                                            item, escapeSequence));
                                        }
                                        return notHex;
                                    })) {
                        break;
                    }
                    unicodeValue = std::stoi(escapeSequence, nullptr, 16);
                    if (unicodeValue <= 0 || unicodeValue > 0x10FFFF) {
                        result.errorReason = ErrorReason::contentError(
                                stringReader.pos.index - escapeSequence.length() - 1, stringReader.pos.index + 1,
                                "字符串转义的Unicode值无效 -> \\u" + escapeSequence);
                        break;
                    }
                    escapeSequence.clear();
                    if (unicodeValue <= 0x7F) {
                        result.result.push_back(static_cast<char>(unicodeValue));
                    } else if (unicodeValue <= 0x7FF) {
                        result.result.push_back(
                                static_cast<char>(0xC0u | (static_cast<unsigned int>(unicodeValue) >> 6u)));
                        result.result.push_back(
                                static_cast<char>(0x80u | (static_cast<unsigned int>(unicodeValue) & 0x3Fu)));
                    } else if (unicodeValue <= 0xFFFF) {
                        result.result.push_back(
                                static_cast<char>(0xE0u | (static_cast<unsigned int>(unicodeValue) >> 12u)));
                        result.result.push_back(
                                static_cast<char>(0x80u | ((static_cast<unsigned int>(unicodeValue) >> 6u) & 0x3Fu)));
                        result.result.push_back(
                                static_cast<char>(0x80u | (static_cast<unsigned int>(unicodeValue) & 0x3Fu)));
                    } else {
                        result.result.push_back(
                                static_cast<char>(0xF0u | (static_cast<unsigned int>(unicodeValue) >> 18u)));
                        result.result.push_back(
                                static_cast<char>(0x80u | ((static_cast<unsigned int>(unicodeValue) >> 12u) & 0x3Fu)));
                        result.result.push_back(
                                static_cast<char>(0x80u | ((static_cast<unsigned int>(unicodeValue) >> 6u) & 0x3Fu)));
                        result.result.push_back(
                                static_cast<char>(0x80u | (static_cast<unsigned int>(unicodeValue) & 0x3Fu)));
                    }
                    result.indexConvertList.push_back(stringReader.pos.index);
                    break;
                default:
                    result.errorReason = ErrorReason::contentError(
                            stringReader.pos.index - 1, stringReader.pos.index + 1,
                            FormatUtil::format("未知的转义字符 -> \\{0}", ch));
                    break;
            }
            if (result.errorReason != nullptr) {
                break;
            }
        }
        return result;
    }

    nlohmann::json getJsonFromPath(const std::filesystem::path &path) {
        Profile::push("reading and parsing json data in path: " + path.string());
        std::ifstream f(path);
        nlohmann::json j = nlohmann::json::parse(f);
        f.close();
        Profile::pop();
        return j;
    }

} // CHelper::JsonUtil