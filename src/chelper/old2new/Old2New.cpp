//
// Created by Yancey on 24-6-12.
//

#include "Old2New.h"

#include "../util/TokenUtil.h"
#include "Old2NewBlockFixData.h"

namespace CHelper::Old2New {

    DataFix::DataFix(size_t start,
                     size_t anEnd,
                     std::string content)
        : start(start),
          end(anEnd),
          content(std::move(content)) {}

    DataFix::DataFix(const VectorView<Token> &tokens,
                     std::string content)
        : start(TokenUtil::getStartIndex(tokens)),
          end(TokenUtil::getEndIndex(tokens)),
          content(std::move(content)) {}

    std::string trip(std::string str) {
        str.erase(0, str.find_first_not_of(' '));
        str.erase(str.find_last_not_of(' ') + 1);
        return str;
    }

    bool expect(TokenReader &tokenReader, const std::function<bool(const Token &token)> &check) {
        tokenReader.push();
        tokenReader.skipWhitespace();
        const Token *token = tokenReader.read();
        if (token == nullptr || !check(*token)) {
            tokenReader.restore();
            return false;
        }
        tokenReader.pop();
        return true;
    }

    bool expectString(TokenReader &tokenReader) {
        return expect(tokenReader, [](const Token &token) {
            return token.type == TokenType::STRING;
        });
    }

    bool expectString(TokenReader &tokenReader, const std::string &str) {
        return expect(tokenReader, [&str](const Token &token) {
            return token.type == TokenType::STRING && token.content == str;
        });
    }

    bool expectSymbol(TokenReader &tokenReader, char ch) {
        return expect(tokenReader, [&ch](const Token &token) {
            return token.type == TokenType::SYMBOL && token.content.length() == 1 && token.content[0] == ch;
        });
    }

    bool expectNumber(TokenReader &tokenReader) {
        return expect(tokenReader, [](const Token &token) {
            return token.type == TokenType::NUMBER;
        });
    }

    bool expectTargetSelector(TokenReader &tokenReader) {
        if (expectString(tokenReader)) {
            return true;
        }
        tokenReader.push();
        if (!expectSymbol(tokenReader, '@')) {
            tokenReader.restore();
            return false;
        }
        if (!expectString(tokenReader)) {
            tokenReader.restore();
            return false;
        }
        if (!expectSymbol(tokenReader, '[')) {
            tokenReader.pop();
            return true;
        }
        size_t left = 1;
        size_t right = 0;
        while (true) {
            if (left == right) {
                tokenReader.pop();
                return true;
            }
            if (expectSymbol(tokenReader, '[')) {
                left++;
                continue;
            }
            if (expectSymbol(tokenReader, ']')) {
                right++;
                continue;
            }
            if (!tokenReader.skip()) {
                tokenReader.restore();
                return false;
            }
        }
    }

    bool expectRelativeFloat(TokenReader &tokenReader) {
        tokenReader.push();
        if (expectSymbol(tokenReader, '~') || expectSymbol(tokenReader, '^')) {
            tokenReader.push();
            if (!tokenReader.skipWhitespace()) {
                tokenReader.pop();
                expectNumber(tokenReader);
            } else {
                tokenReader.restore();
            }
            tokenReader.pop();
            return true;
        }
        if (!expectNumber(tokenReader)) {
            tokenReader.restore();
            return false;
        } else {
            tokenReader.pop();
            return true;
        }
    }

    bool expectPosition(TokenReader &tokenReader) {
        tokenReader.push();
        if (!expectRelativeFloat(tokenReader)) {
            tokenReader.restore();
            return false;
        }
        if (!expectRelativeFloat(tokenReader)) {
            tokenReader.restore();
            return false;
        }
        if (!expectRelativeFloat(tokenReader)) {
            tokenReader.restore();
            return false;
        }
        tokenReader.pop();
        return true;
    }

    std::string blockOld2New(const VectorView<Token> &blockIdToken, const VectorView<Token> &dataValueToken) {
        // get block id
        std::string blockId = TokenUtil::toString(blockIdToken);
        // get block data value
        char *end;
        std::string dataValueStr = TokenUtil::toString(dataValueToken);
        std::intmax_t dataValue = std::strtoimax(dataValueStr.c_str(), &end, 10);
        if (HEDLEY_UNLIKELY(end == blockId.c_str() || *end != '\0' ||
                            dataValue == HUGE_VALF || dataValue == -HUGE_VALF ||
                            dataValue < 0)) {
            // if it is not an integer or in range, return block id directly
            return blockId;
        }
        // get key
        std::string front = "minecraft:";
        std::string key = trip(blockId);
        if (key.size() < front.size() || key.substr(0, front.size()) != front) {
            key = front + key;
        }
        key = key + "|" + std::to_string(dataValue);
        // find fixed block state by key
        auto iter = blockFixData.find(key);
        // if it doesn't need to fix, return block id directly
        if (iter == blockFixData.end()) {
            return blockId;
        }
        // get block state
        std::string blockState = iter->get<std::string>();
        if (blockState == "[]") {
            return blockId;
        }
        size_t index = 0;
        while ((index = blockState.find(' ', index)) != std::string::npos) {
            blockState.erase(index, 1);
        }
        size_t index2 = 1;
        while ((index2 = blockState.find(':', index2)) != std::string::npos) {
            blockState[index2] = '=';
        }
        return blockId + blockState;
    }

    /**
     * execute命令转为新语法
     * 旧语法例子：execute @e ~~~ detect ~~-1~ stone 0 run setblock command_block ~~~
     * 新语法例子：execute as @e at @s positioned ~~~ if block ~~-1~ stone setblock command_block ~~~
     */
    bool expectCommandExecute(TokenReader &tokenReader, std::vector<DataFix> &dataFixList, size_t depth) {
        tokenReader.push();
        // execute
        tokenReader.push();
        if (!expectString(tokenReader, "execute")) {
            tokenReader.restore();
            tokenReader.restore();
            return false;
        }
        VectorView<Token> tokens1 = tokenReader.collect();
        // @e
        tokenReader.push();
        if (!expectTargetSelector(tokenReader)) {
            tokenReader.restore();
            tokenReader.restore();
            return false;
        }
        VectorView<Token> tokens2 = tokenReader.collect();
        // ~~~
        tokenReader.push();
        if (!expectPosition(tokenReader)) {
            tokenReader.restore();
            tokenReader.restore();
            return false;
        }
        VectorView<Token> tokens3 = tokenReader.collect();
        // end?
        if (depth > 0) {
            dataFixList.emplace_back(tokens1, "");
        }
        std::string targetSelector = TokenUtil::toString(tokens2);
        if (trip(targetSelector) != "@s") {
            dataFixList.emplace_back(tokens2, " as" + targetSelector + " at @s");
        } else {
            dataFixList.emplace_back(tokens2, "");
        }
        bool isHavePosition = false;
        if (depth > 0) {
            tokens3.forEach([&isHavePosition](const Token &token) {
                if (token.type == TokenType::NUMBER) {
                    isHavePosition = true;
                }
            });
        }
        if (isHavePosition) {
            dataFixList.emplace_back(tokens3, " positioned" + TokenUtil::toString(tokens3));
        } else {
            dataFixList.emplace_back(tokens3, "");
        }
        // detect
        tokenReader.push();
        if (!expectString(tokenReader, "detect")) {
            tokenReader.pop();
            tokenReader.pop();
            return true;
        }
        VectorView<Token> tokens4 = tokenReader.collect();
        // ~~-1~
        if (!expectPosition(tokenReader)) {
            tokenReader.restore();
            return false;
        }
        // stone
        tokenReader.push();
        if (!expectString(tokenReader)) {
            tokenReader.restore();
            tokenReader.restore();
            return true;
        }
        VectorView<Token> tokens5 = tokenReader.collect();
        // 0
        tokenReader.push();
        if (!expectNumber(tokenReader)) {
            tokenReader.restore();
            tokenReader.restore();
            return true;
        }
        VectorView<Token> tokens6 = tokenReader.collect();
        // end
        dataFixList.emplace_back(tokens4, " if block");
        dataFixList.emplace_back(tokens5, blockOld2New(tokens5, tokens6));
        dataFixList.emplace_back(tokens6, "");
        tokenReader.pop();
        return true;
    }

    bool expectCommandExecuteRepeat(TokenReader &tokenReader, std::vector<DataFix> &dataFixList) {
        size_t depth = 0;
        while (true) {
            tokenReader.push();
            expectSymbol(tokenReader, '/');
            if (!expectCommandExecute(tokenReader, dataFixList, depth)) {
                tokenReader.restore();
                break;
            }
            tokenReader.pop();
            depth++;
        }
        if (depth == 0) {
            return false;
        }
        tokenReader.push();
        tokenReader.skipWhitespace();
        VectorView<Token> tokens = tokenReader.collect();
        dataFixList.emplace_back(tokens, " run ");
        return true;
    }

    /**
     * setblock命令转为新语法
     * 旧语法例子：setblock ~~~ stone 1 replace
     * 新语法例子：setblock ~~~ stone["stone_type":"granite"] replace
     */
    bool expectCommandSetBlock(TokenReader &tokenReader, std::vector<DataFix> &dataFixList) {
        tokenReader.push();
        // setblock
        if (!expectString(tokenReader, "setblock")) {
            tokenReader.restore();
            return false;
        }
        // ~~~
        if (!expectPosition(tokenReader)) {
            tokenReader.restore();
            return false;
        }
        // stone
        tokenReader.push();
        if (!expectString(tokenReader)) {
            tokenReader.restore();
            tokenReader.restore();
            return true;
        }
        VectorView<Token> tokens1 = tokenReader.collect();
        // 0
        tokenReader.push();
        if (!expectNumber(tokenReader)) {
            tokenReader.pop();
            tokenReader.pop();
            return true;
        }
        VectorView<Token> tokens2 = tokenReader.collect();
        // replace
        tokenReader.skipToLF();
        // end
        dataFixList.emplace_back(tokens1, blockOld2New(tokens1, tokens2));
        dataFixList.emplace_back(tokens2, "");
        tokenReader.pop();
        return true;
    }

    /**
     * fill命令转为新语法
     * 旧语法例子：fill ~~~~~~ stone 1 replace stone 2
     * 新语法例子：fill ~~~~~~ stone["stone_type":"granite"] replace stone["stone_type":"granite_smooth"]
     */
    bool expectCommandFill(TokenReader &tokenReader, std::vector<DataFix> &dataFixList) {
        tokenReader.push();
        // fill
        if (!expectString(tokenReader, "fill")) {
            tokenReader.restore();
            return false;
        }
        // ~~~
        if (!expectPosition(tokenReader)) {
            tokenReader.restore();
            return false;
        }
        // ~~~
        if (!expectPosition(tokenReader)) {
            tokenReader.restore();
            return false;
        }
        // stone
        tokenReader.push();
        if (!expectString(tokenReader)) {
            tokenReader.restore();
            tokenReader.restore();
            return true;
        }
        VectorView<Token> tokens1 = tokenReader.collect();
        // 0
        tokenReader.push();
        if (!expectNumber(tokenReader)) {
            tokenReader.pop();
            tokenReader.pop();
            return true;
        }
        VectorView<Token> tokens2 = tokenReader.collect();
        // end?
        dataFixList.emplace_back(tokens1, blockOld2New(tokens1, tokens2));
        dataFixList.emplace_back(tokens2, "");
        tokenReader.pop();
        // replace
        if (!expectString(tokenReader, "replace")) {
            return true;
        }
        // stone
        tokenReader.push();
        if (!expectString(tokenReader)) {
            tokenReader.pop();
            return true;
        }
        VectorView<Token> tokens3 = tokenReader.collect();
        // 0
        tokenReader.push();
        if (!expectNumber(tokenReader)) {
            tokenReader.pop();
            return true;
        }
        VectorView<Token> tokens4 = tokenReader.collect();
        // end
        dataFixList.emplace_back(tokens3, blockOld2New(tokens3, tokens4));
        dataFixList.emplace_back(tokens4, "");
        return true;
    }

    /**
     * testforblock命令转为新语法
     * 旧语法例子：testforblock ~~~ stone 1
     * 新语法例子：testforblock ~~~ stone["stone_type":"granite"]
     */
    bool expectCommandTestForSetBlock(TokenReader &tokenReader, std::vector<DataFix> &dataFixList) {
        tokenReader.push();
        // testforblock
        if (!expectString(tokenReader, "testforblock")) {
            tokenReader.restore();
            return false;
        }
        // ~~~
        if (!expectPosition(tokenReader)) {
            tokenReader.restore();
            return false;
        }
        // stone
        tokenReader.push();
        if (!expectString(tokenReader)) {
            tokenReader.restore();
            tokenReader.restore();
            return true;
        }
        VectorView<Token> tokens1 = tokenReader.collect();
        // 0
        tokenReader.push();
        if (!expectNumber(tokenReader)) {
            tokenReader.pop();
            tokenReader.pop();
            return true;
        }
        VectorView<Token> tokens2 = tokenReader.collect();
        // end
        dataFixList.emplace_back(tokens1, blockOld2New(tokens1, tokens2));
        dataFixList.emplace_back(tokens2, "");
        tokenReader.pop();
        return true;
    }

    bool expectCommand(TokenReader &tokenReader, std::vector<DataFix> &dataFixList) {
        if (!tokenReader.ready()) {
            return false;
        }
        expectCommandExecuteRepeat(tokenReader, dataFixList);
        expectSymbol(tokenReader, '/');
        expectCommandFill(tokenReader, dataFixList);
        expectCommandSetBlock(tokenReader, dataFixList);
        expectCommandTestForSetBlock(tokenReader, dataFixList);
        tokenReader.skipToLF();
        return true;
    }

    std::string old2new(const std::string &old) {
        TokenReader tokenReader(std::make_shared<std::vector<Token>>(Lexer::lex(StringReader(old, "unknown"))));
        std::vector<DataFix> dataFixList;
        expectCommand(tokenReader, dataFixList);
        std::sort(dataFixList.begin(), dataFixList.end(), [](const DataFix &dataFix1, const DataFix &dataFix2) {
            return dataFix1.start < dataFix2.start;
        });
        std::string result;
        size_t index = 0;
        for (const auto &item: dataFixList) {
            if (item.start > index) {
                result.append(old.substr(index, item.start - index));
            }
            result.append(item.content);
            index = item.end;
        }
        result.append(old.substr(index));
        return result;
    }

}// namespace CHelper::Old2New
