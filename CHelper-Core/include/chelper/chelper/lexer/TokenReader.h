//
// Created by Yancey on 2023/12/15.
//

#pragma once

#ifndef CHELPER_TOKENREADER_H
#define CHELPER_TOKENREADER_H

#include "../parser/ASTNode.h"
#include "../parser/TokensView.h"
#include "LexerResult.h"
#include "Token.h"
#include "pch.h"

namespace CHelper {

    namespace Node {

        class NodeBase;

    }// namespace Node

    class TokenReader {
    public:
        const std::shared_ptr<LexerResult> lexerResult;
        size_t index = 0;
        std::vector<size_t> indexStack;

        explicit TokenReader(const std::shared_ptr<LexerResult> &lexerResult);

        [[nodiscard]] bool ready() const;

        [[nodiscard]] const Token *peek() const;

        [[nodiscard]] const Token *read();

        [[nodiscard]] const Token *next();

        bool skip();

        size_t skipSpace();

        void skipToLF();

        void push();

        void pop();

        [[nodiscard]] size_t getAndPopLastIndex();

        void restore();

        [[nodiscard]] TokensView collect();

        ASTNode readSimpleASTNode(const Node::NodeBase *node,
                                  TokenType::TokenType type,
                                  const std::u16string &requireType,
                                  const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE,
                                  std::shared_ptr<ErrorReason> (*check)(const std::u16string_view &str,
                                                                        const TokensView &tokens) = nullptr);

        ASTNode readStringASTNode(const Node::NodeBase *node,
                                  const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE);

        ASTNode readIntegerASTNode(const Node::NodeBase *node,
                                   const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE);

        ASTNode readFloatASTNode(const Node::NodeBase *node,
                                 const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE);

        ASTNode readSymbolASTNode(const Node::NodeBase *node,
                                  const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE);

        ASTNode readUntilSpace(const Node::NodeBase *node,
                               const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE);

        ASTNode readStringOrNumberASTNode(const Node::NodeBase *node,
                                          const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE);
    };

}// namespace CHelper

#endif//CHELPER_TOKENREADER_H
