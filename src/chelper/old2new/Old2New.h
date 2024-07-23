//
// Created by Yancey on2024-6-12.
//

#ifndef CHELPER_OLD2NEW_H
#define CHELPER_OLD2NEW_H

#include "../lexer/Lexer.h"
#include "../lexer/TokenReader.h"
#include "pch.h"

namespace CHelper::Old2New {

    typedef std::unordered_map<std::string, std::unordered_map<uint32_t, std::pair<std::optional<std::string>, std::optional<std::string>>>> BlockFixData;

    class DataFix {
    public:
        size_t start, end;
        std::string content;

        DataFix(size_t start, size_t anEnd, std::string content);

        DataFix(const TokensView &tokens, std::string content);
    };

    bool expect(TokenReader &tokenReader, const std::function<bool(const Token &token)> &check);

    bool expectString(TokenReader &tokenReader);

    bool expectString(TokenReader &tokenReader, const std::string &str);

    bool expectSymbol(TokenReader &tokenReader, char ch);

    bool expectNumber(TokenReader &tokenReader);

    bool expectTargetSelector(TokenReader &tokenReader);

    bool expectRelativeFloat(TokenReader &tokenReader);

    bool expectPosition(TokenReader &tokenReader);

    std::string blockOld2New(const BlockFixData &blockFixData, const TokensView &blockIdToken, const TokensView &dataValueToken);

    bool expectCommandExecute(const BlockFixData &blockFixData, TokenReader &tokenReader, std::vector<DataFix> &dataFixList, size_t depth);

    bool expectCommandExecuteRepeat(const BlockFixData &blockFixData, TokenReader &tokenReader, std::vector<DataFix> &dataFixList);

    bool expectCommandSummon(TokenReader &tokenReader, std::vector<DataFix> &dataFixList);

    bool expectCommandStructure(TokenReader &tokenReader, std::vector<DataFix> &dataFixList);

    bool expectCommandSetBlock(const BlockFixData &blockFixData, TokenReader &tokenReader, std::vector<DataFix> &dataFixList);

    bool expectCommandFill(const BlockFixData &blockFixData, TokenReader &tokenReader, std::vector<DataFix> &dataFixList);

    bool expectCommandTestForSetBlock(const BlockFixData &blockFixData, TokenReader &tokenReader, std::vector<DataFix> &dataFixList);

    bool expectCommand(const BlockFixData &blockFixData, TokenReader &tokenReader, std::vector<DataFix> &dataFixList);

    std::string old2new(const BlockFixData &blockFixData, const std::string &old);

    BlockFixData blockFixDataFromJson(const nlohmann::json &j);

}// namespace CHelper::Old2New

#endif//CHELPER_OLD2NEW_H
