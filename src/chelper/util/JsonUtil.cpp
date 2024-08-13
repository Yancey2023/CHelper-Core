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
        StringReader stringReader(input);
        while (true) {
            signed char ch = stringReader.read();
            switch (ch) {
                case EOF:
                    return std::move(result);
                case '\"':
                case '\\':
                case '/':
                case '\b':
                case '\f':
                case '\n':
                case '\r':
                case '\t':
                    result.push_back('\\');
                    break;
                default:
                    break;
            }
            result.push_back(ch);
        }
    }

    ConvertResult jsonString2String(const std::string &input) {
        ConvertResult result;
        if (HEDLEY_UNLIKELY(input.empty())) {
            result.errorReason = ErrorReason::incomplete(0, 0, "json字符串必须在双引号内");
            return std::move(result);
        }
        StringReader stringReader(input);
        result.indexConvertList.push_back(stringReader.pos.index);
        int32_t unicodeValue;
        std::string escapeSequence;
        while (true) {
            signed char ch = stringReader.next();
            //结束字符
            if (HEDLEY_UNLIKELY(ch == EOF)) {
                result.isComplete = false;
                break;
            } else if (HEDLEY_UNLIKELY(ch == '\"')) {
                result.isComplete = true;
                break;
            }
            //正常字符
            if (HEDLEY_LIKELY(ch != '\\')) {
                result.result.push_back(ch);
                result.indexConvertList.push_back(stringReader.pos.index);
                continue;
            }
            //转义字符
            ch = stringReader.next();
            switch (ch) {
                case EOF:
                    result.errorReason = ErrorReason::incomplete(
                            stringReader.pos.index - 1,
                            stringReader.pos.index,
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
                    for (uint8_t i = 0; i < 4; ++i) {
                        ch = stringReader.next();
                        if (HEDLEY_UNLIKELY(ch == EOF)) {
                            result.errorReason = ErrorReason::contentError(
                                    stringReader.pos.index - 2 - i,
                                    stringReader.pos.index,
                                    fmt::format("字符串转义缺失后半部分 -> \\u{}", escapeSequence));
                            break;
                        }
                        escapeSequence.push_back(ch);
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
                                                fmt::format("字符串转义出现非法字符{} -> \\u{}", item, escapeSequence));
                                        return true;
                                    }
                                }))) {
                        break;
                    }
                    unicodeValue = std::stoi(escapeSequence, nullptr, 16);
                    if (HEDLEY_UNLIKELY(unicodeValue <= 0 || unicodeValue > 0x10FFFF)) {
                        result.errorReason = ErrorReason::contentError(
                                stringReader.pos.index - escapeSequence.length() - 1, stringReader.pos.index + 1,
                                fmt::format("字符串转义的Unicode值无效 -> \\u{}", escapeSequence));
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
                            fmt::format("未知的转义字符 -> \\{:c}", ch));
                    break;
            }
            if (HEDLEY_UNLIKELY(result.errorReason != nullptr)) {
                break;
            }
        }
        return std::move(result);
    }

#if CHelperOnlyReadBinary != true
    nlohmann::json getJsonFromFile(const std::filesystem::path &path) {
        Profile::push("reading and parsing json in file: {}", path.string());
        std::ifstream f(path);
        nlohmann::json j = nlohmann::json::parse(f);
        f.close();
        Profile::pop();
        return std::move(j);
    }

    nlohmann::json getBsonFromFile(const std::filesystem::path &path) {
        Profile::push("reading and parsing bjdata in file: {}", path.string());
        std::ifstream f(path, std::ios::binary);
        nlohmann::json j = nlohmann::json::from_bjdata(f);
        f.close();
        Profile::pop();
        return std::move(j);
    }

    void writeJsonToFile(const std::filesystem::path &path, const nlohmann::json &j) {
        std::filesystem::create_directories(path.parent_path());
        Profile::push("writing json in file: {}", path.string());
        std::ofstream f(path);
        f << j;
        f.close();
        Profile::pop();
    }

    void writeBsonToFile(const std::filesystem::path &path, const nlohmann::json &j) {
        std::filesystem::create_directories(path.parent_path());
        Profile::push("writing bjdata in file: {}", path.string());
        std::ofstream f(path, std::ios::binary);
        nlohmann::json::to_bjdata(j, f);
        f.close();
        Profile::pop();
    }
#endif

}// namespace CHelper::JsonUtil