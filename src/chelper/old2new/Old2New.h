//
// Created by Yancey on 24-6-12.
//

#ifndef CHELPER_OLD2NEW_H
#define CHELPER_OLD2NEW_H

#include "../lexer/Lexer.h"
#include "../parser/TokenReader.h"
#include "pch.h"

namespace CHelper::Old2New {

    class DataFix {
    public:
        size_t start, end;
        std::string content;

        DataFix(size_t start, size_t anEnd, std::string content);

        DataFix(const VectorView<Token> &tokens, std::string content);
    };

    bool expect(TokenReader &tokenReader, const std::function<bool(const Token &token)> &check);

    bool expectString(TokenReader &tokenReader);

    bool expectString(TokenReader &tokenReader, const std::string &str);

    bool expectSymbol(TokenReader &tokenReader, char ch);

    bool expectNumber(TokenReader &tokenReader);

    bool expectTargetSelector(TokenReader &tokenReader);

    bool expectRelativeFloat(TokenReader &tokenReader);

    bool expectPosition(TokenReader &tokenReader);

    std::string blockOld2New(const VectorView<Token> &blockIdToken, const VectorView<Token> &dataValueToken);

    bool expectCommandExecute(TokenReader &tokenReader, std::vector<DataFix> &dataFixList, size_t depth);

    bool expectCommandExecuteRepeat(TokenReader &tokenReader, std::vector<DataFix> &dataFixList);

    bool expectCommandSetBlock(TokenReader &tokenReader, std::vector<DataFix> &dataFixList);

    bool expectCommandFill(TokenReader &tokenReader, std::vector<DataFix> &dataFixList);

    bool expectCommandTestForSetBlock(TokenReader &tokenReader, std::vector<DataFix> &dataFixList);

    bool expectCommand(TokenReader &tokenReader, std::vector<DataFix> &dataFixList);

    std::string old2new(const std::string &old);

}// namespace CHelper::Old2New

#endif//CHELPER_OLD2NEW_H
