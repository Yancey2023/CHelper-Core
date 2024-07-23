//
// Created by Yancey on2024-6-12.
//

#include "Old2New.h"

namespace CHelper::Old2New {

    DataFix::DataFix(size_t start,
                     size_t anEnd,
                     std::string content)
        : start(start),
          end(anEnd),
          content(std::move(content)) {}

    DataFix::DataFix(const TokensView &tokens,
                     std::string content)
        : start(tokens.getStartIndex()),
          end(tokens.getEndIndex()),
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

    bool expectList(TokenReader &tokenReader, char leftSymbol, char rightSymbol) {
        tokenReader.push();
        if (!expectSymbol(tokenReader, leftSymbol)) {
            tokenReader.restore();
            return false;
        }
        size_t left = 1;
        size_t right = 0;
        while (true) {
            if (left == right) {
                tokenReader.pop();
                return true;
            }
            if (expectSymbol(tokenReader, leftSymbol)) {
                left++;
                continue;
            }
            if (expectSymbol(tokenReader, rightSymbol)) {
                right++;
                continue;
            }
            if (!tokenReader.skip()) {
                tokenReader.restore();
                return false;
            }
        }
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
        expectList(tokenReader, '[', ']');
        return true;
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

    std::string blockOld2New(const BlockFixData &blockFixData, const TokensView &blockIdToken, const TokensView &dataValueToken) {
        // get block id
        std::string blockId = std::string(blockIdToken.toString());
        // get block data value
        char *end;
        std::string dataValueStr = std::string(dataValueToken.toString());
        std::intmax_t dataValue = std::strtoimax(dataValueStr.c_str(), &end, 10);
        if (HEDLEY_UNLIKELY(end == dataValueStr.c_str() || *end != '\0' ||
                            dataValue == HUGE_VALF || dataValue == -HUGE_VALF ||
                            dataValue < 0)) {
            // if it is not an integer or in range, return block id directly
            return blockId;
        }
        // get key
        std::string front = "minecraft:";
        std::string key = trip(blockId);
        bool isStartWithMinecraft = key.size() > front.size() && key.substr(0, front.size()) == front;
        if (isStartWithMinecraft) {
            key = key.substr(front.size());
        }
        // find fixed block state by key
        auto blockIdIter = blockFixData.find(key);
        if (blockIdIter == blockFixData.end()) {
            return blockId;
        }
        const auto &dataValueToBlockState = blockIdIter->second;
        auto dataValueIter = dataValueToBlockState.find(dataValue);
        if (dataValueIter == dataValueToBlockState.end()) {
            return blockId;
        }
        const auto &blockIdWithBlockState = dataValueIter->second;
        if (blockIdWithBlockState.first.has_value()) {
            return front + blockIdWithBlockState.first.value() + blockIdWithBlockState.second.value_or("");
        } else {
            return blockIdWithBlockState.first.value_or(blockId) + blockIdWithBlockState.second.value_or("");
        }
    }

    /**
     * execute命令转为新语法
     * 旧语法例子：execute @e ~~~ detect ~~-1~ stone 0 run setblock command_block ~~~
     * 新语法例子：execute as @e at @s positioned ~~~ if block ~~-1~ stone setblock command_block ~~~
     */
    bool expectCommandExecute(const BlockFixData &blockFixData, TokenReader &tokenReader, std::vector<DataFix> &dataFixList, size_t depth) {
        tokenReader.push();
        // execute
        tokenReader.push();
        if (!expectString(tokenReader, "execute")) {
            tokenReader.restore();
            tokenReader.restore();
            return false;
        }
        TokensView tokens1 = tokenReader.collect();
        // @e
        tokenReader.push();
        if (!expectTargetSelector(tokenReader)) {
            tokenReader.restore();
            tokenReader.restore();
            return false;
        }
        TokensView tokens2 = tokenReader.collect();
        // ~~~
        tokenReader.push();
        if (!expectPosition(tokenReader)) {
            tokenReader.restore();
            tokenReader.restore();
            return false;
        }
        TokensView tokens3 = tokenReader.collect();
        // end?
        tokenReader.pop();
        if (depth > 0) {
            dataFixList.emplace_back(tokens1, "");
        }
        std::string targetSelector = std::string(tokens2.toString());
        if (trip(targetSelector) != "@s") {
            dataFixList.emplace_back(tokens2, " as" + targetSelector + " at @s");
        } else {
            dataFixList.emplace_back(tokens2, "");
        }
        bool isHavePosition = false;
        tokens3.forEach([&isHavePosition](const Token &token) {
            if (token.type == TokenType::NUMBER) {
                isHavePosition = true;
            }
        });
        if (isHavePosition) {
            dataFixList.emplace_back(tokens3, " positioned" + std::string(tokens3.toString()));
        } else {
            dataFixList.emplace_back(tokens3, "");
        }
        tokenReader.push();
        // detect
        tokenReader.push();
        if (!expectString(tokenReader, "detect")) {
            tokenReader.pop();
            return true;
        }
        TokensView tokens4 = tokenReader.collect();
        // ~~-1~
        if (!expectPosition(tokenReader)) {
            tokenReader.restore();
            return true;
        }
        // stone
        tokenReader.push();
        if (!expectString(tokenReader)) {
            tokenReader.restore();
            tokenReader.restore();
            return true;
        }
        TokensView tokens5 = tokenReader.collect();
        // 0
        tokenReader.push();
        if (expectNumber(tokenReader)) {
            TokensView tokens6 = tokenReader.collect();
            dataFixList.emplace_back(tokens4, " if block");
            dataFixList.emplace_back(tokens5, blockOld2New(blockFixData, tokens5, tokens6));
            dataFixList.emplace_back(tokens6, "");
        } else if (expectSymbol(tokenReader, '[')) {
            tokenReader.restore();
            tokenReader.push();
            expectList(tokenReader, '[', ']');
            TokensView tokens6 = tokenReader.collect();
            dataFixList.emplace_back(tokens4, " if block");
            tokens6.forEach([&dataFixList](const Token &token) {
                if (token.type == TokenType::SYMBOL && token.content == ":") {
                    dataFixList.emplace_back(token.getStartIndex(), token.getEndIndex(), "=");
                }
            });
        } else {
            tokenReader.restore();
            tokenReader.restore();
            return true;
        }
        // end
        tokenReader.pop();
        return true;
    }

    bool expectCommandExecuteRepeat(const BlockFixData &blockFixData, TokenReader &tokenReader, std::vector<DataFix> &dataFixList) {
        size_t depth = 0;
        while (true) {
            tokenReader.push();
            expectSymbol(tokenReader, '/');
            if (!expectCommandExecute(blockFixData, tokenReader, dataFixList, depth)) {
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
        TokensView tokens = tokenReader.collect();
        dataFixList.emplace_back(tokens, " run ");
        return true;
    }

    /**
     * summon命令转为新语法
     * 旧语法例子：summon creeper ~ ~ ~ minecraft:become_charged "充能苦力怕"
     * 新语法例子：summon creeper ~ ~ ~ ~ ~ minecraft:become_charged "充能苦力怕"
     */
    bool expectCommandSummon(TokenReader &tokenReader, std::vector<DataFix> &dataFixList) {
        tokenReader.push();
        // summon
        if (!expectString(tokenReader, "summon")) {
            tokenReader.restore();
            return false;
        }
        // creeper
        if (!expectString(tokenReader)) {
            tokenReader.restore();
            return false;
        }
        // end?
        tokenReader.pop();
        // ~~~
        if (!expectPosition(tokenReader)) {
            return true;
        }
        // end?
        tokenReader.push();
        TokensView tokens1 = tokenReader.collect();
        dataFixList.emplace_back(tokens1, " 0 0");
        // minecraft:become_charged
        if (!expectString(tokenReader)) {
            return true;
        }
        // "充能苦力怕"
        if (!expectString(tokenReader)) {
            return true;
        }
        // end
        return true;
    }

    /**
     * structure命令转为新语法
     * 旧语法例子：structure load <name: string> <to:x y z> [rotation: Rotation] [mirror: Mirror] [includeEntities: Boolean] [includeBlocks: Boolean] [integrity: float] [seed: string]
     * 新语法例子：structure load <name: string> <to:x y z> [rotation: Rotation] [mirror: Mirror] [includeEntities: Boolean] [includeBlocks: Boolean] [waterlogged: Boolean] [integrity: float] [seed: string]
     */
    bool expectCommandStructure(TokenReader &tokenReader, std::vector<DataFix> &dataFixList) {
        tokenReader.push();
        // structure
        if (!expectString(tokenReader, "structure")) {
            tokenReader.restore();
            return false;
        }
        // load
        if (!expectString(tokenReader, "load")) {
            tokenReader.restore();
            return false;
        }
        // end?
        tokenReader.pop();
        // <name: string>
        if (!expectString(tokenReader)) {
            return false;
        }
        // <to:x y z>
        if (!expectPosition(tokenReader)) {
            return false;
        }
        // [rotation: Rotation]
        if (!expectNumber(tokenReader)) {
            return true;
        }
        if (!expectString(tokenReader)) {
            return true;
        }
        // [mirror: Mirror]
        if (!expectString(tokenReader)) {
            return true;
        }
        // [includeEntities: Boolean]
        if (!expectString(tokenReader)) {
            return true;
        }
        // [includeBlocks: Boolean]
        if (!expectString(tokenReader)) {
            return true;
        }
        // end?
        tokenReader.push();
        TokensView tokens1 = tokenReader.collect();
        dataFixList.emplace_back(tokens1, " true");
        // [integrity: float]
        if (!expectNumber(tokenReader)) {
            return true;
        }
        // [seed: string]
        if (!expectString(tokenReader)) {
            return true;
        }
        // end
        return true;
    }

    /**
     * setblock命令转为新语法
     * 旧语法例子：setblock ~~~ stone 1 replace
     * 新语法例子：setblock ~~~ stone["stone_type":"granite"] replace
     */
    bool expectCommandSetBlock(const BlockFixData &blockFixData, TokenReader &tokenReader, std::vector<DataFix> &dataFixList) {
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
        TokensView tokens1 = tokenReader.collect();
        // 0
        tokenReader.push();
        if (expectNumber(tokenReader)) {
            TokensView tokens2 = tokenReader.collect();
            dataFixList.emplace_back(tokens1, blockOld2New(blockFixData, tokens1, tokens2));
            dataFixList.emplace_back(tokens2, "");
        } else if (expectSymbol(tokenReader, '[')) {
            tokenReader.restore();
            tokenReader.push();
            expectList(tokenReader, '[', ']');
            TokensView tokens2 = tokenReader.collect();
            tokens2.forEach([&dataFixList](const Token &token) {
                if (token.type == TokenType::SYMBOL && token.content == ":") {
                    dataFixList.emplace_back(token.getStartIndex(), token.getEndIndex(), "=");
                }
            });
        } else {
            tokenReader.restore();
            tokenReader.restore();
            return true;
        }
        // replace
        tokenReader.skipToLF();
        // end
        tokenReader.pop();
        return true;
    }

    /**
     * fill命令转为新语法
     * 旧语法例子：fill ~~~~~~ stone 1 replace stone 2
     * 新语法例子：fill ~~~~~~ stone["stone_type":"granite"] replace stone["stone_type":"granite_smooth"]
     */
    bool expectCommandFill(const BlockFixData &blockFixData, TokenReader &tokenReader, std::vector<DataFix> &dataFixList) {
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
        TokensView tokens1 = tokenReader.collect();
        // end?
        tokenReader.pop();
        // 0
        tokenReader.push();
        if (expectNumber(tokenReader)) {
            TokensView tokens2 = tokenReader.collect();
            dataFixList.emplace_back(tokens1, blockOld2New(blockFixData, tokens1, tokens2));
            dataFixList.emplace_back(tokens2, "");
        } else if (expectSymbol(tokenReader, '[')) {
            tokenReader.restore();
            tokenReader.push();
            expectList(tokenReader, '[', ']');
            TokensView tokens2 = tokenReader.collect();
            tokens2.forEach([&dataFixList](const Token &token) {
                if (token.type == TokenType::SYMBOL && token.content == ":") {
                    dataFixList.emplace_back(token.getStartIndex(), token.getEndIndex(), "=");
                }
            });
        } else {
            tokenReader.restore();
            tokenReader.restore();
            return true;
        }
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
        TokensView tokens3 = tokenReader.collect();
        // 0
        tokenReader.push();
        if (expectNumber(tokenReader)) {
            TokensView tokens4 = tokenReader.collect();
            dataFixList.emplace_back(tokens3, blockOld2New(blockFixData, tokens3, tokens4));
            dataFixList.emplace_back(tokens4, "");
        } else if (expectSymbol(tokenReader, '[')) {
            tokenReader.restore();
            tokenReader.push();
            expectList(tokenReader, '[', ']');
            TokensView tokens4 = tokenReader.collect();
            tokens4.forEach([&dataFixList](const Token &token) {
                if (token.type == TokenType::SYMBOL && token.content == ":") {
                    dataFixList.emplace_back(token.getStartIndex(), token.getEndIndex(), "=");
                }
            });
        } else {
            tokenReader.restore();
            tokenReader.restore();
            return true;
        }
        return true;
    }

    /**
     * testforblock命令转为新语法
     * 旧语法例子：testforblock ~~~ stone 1
     * 新语法例子：testforblock ~~~ stone["stone_type":"granite"]
     */
    bool expectCommandTestForSetBlock(const BlockFixData &blockFixData, TokenReader &tokenReader, std::vector<DataFix> &dataFixList) {
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
        TokensView tokens1 = tokenReader.collect();
        // 0
        tokenReader.push();
        if (!expectNumber(tokenReader)) {
            tokenReader.pop();
            tokenReader.pop();
            return true;
        }
        TokensView tokens2 = tokenReader.collect();
        // end
        dataFixList.emplace_back(tokens1, blockOld2New(blockFixData, tokens1, tokens2));
        dataFixList.emplace_back(tokens2, "");
        tokenReader.pop();
        return true;
    }

    bool expectCommand(const BlockFixData &blockFixData, TokenReader &tokenReader, std::vector<DataFix> &dataFixList) {
        if (!tokenReader.ready()) {
            return false;
        }
        expectCommandExecuteRepeat(blockFixData, tokenReader, dataFixList);
        expectSymbol(tokenReader, '/');
        expectCommandFill(blockFixData, tokenReader, dataFixList);
        expectCommandSetBlock(blockFixData, tokenReader, dataFixList);
        expectCommandSummon(tokenReader, dataFixList);
        expectCommandStructure(tokenReader, dataFixList);
        expectCommandTestForSetBlock(blockFixData, tokenReader, dataFixList);
        tokenReader.skipToLF();
        return true;
    }

    std::string old2new(const BlockFixData &blockFixData, const std::string &old) {
        TokenReader tokenReader(std::make_shared<LexerResult>(Lexer::lex(old)));
        std::vector<DataFix> dataFixList;
        expectCommand(blockFixData, tokenReader, dataFixList);
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

#if CHelperSupportJson == true
    template<class T, class S>
    S &getOrCreate(std::unordered_map<T, S> &map, const T &key) {
        const auto &iter = map.find(key);
        if (iter != map.end()) {
            return iter->second;
        }
        return map.emplace(key, S()).first->second;
    }

    BlockFixData blockFixDataFromJson(const nlohmann::json &j) {
        BlockFixData blockFixData;
        for (const auto &item: j) {
            const auto name = JsonUtil::read<std::string>(item, "name");
            const auto data = JsonUtil::read<uint32_t>(item, "data");
            const auto newBlockId = JsonUtil::read<std::optional<std::string>>(item, "newBlockId");
            const auto blockState = JsonUtil::read<std::optional<std::string>>(item, "blockState");
            getOrCreate(blockFixData, name).insert({data, {newBlockId, blockState}});
        }
        return blockFixData;
    }
#endif

}// namespace CHelper::Old2New
