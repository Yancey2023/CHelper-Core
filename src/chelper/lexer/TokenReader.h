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

#if CHelperDebug == true
#define DEBUG_GET_NODE_BEGIN(node) size_t node##Index = tokenReader.indexStack.size();
#else
#define DEBUG_GET_NODE_BEGIN(node)
#endif

#if CHelperDebug == true
#define DEBUG_GET_NODE_END(node)                                         \
    if (HEDLEY_UNLIKELY(node##Index != tokenReader.indexStack.size())) { \
        Profile::push(L"TokenReaderIndexError: {} {} {}",                \
                      (node)->getNodeType()->nodeName,                   \
                      (node)->id.value_or(L""),                          \
                      (node)->description.value_or(L""));                \
        throw std::runtime_error("TokenReaderIndexError");               \
    }
#else
#define DEBUG_GET_NODE_END(node)
#endif

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

        size_t skipWhitespace();

        void skipToLF();

        void push();

        void pop();

        [[nodiscard]] size_t getAndPopLastIndex();

        void restore();

        [[nodiscard]] TokensView collect();

        ASTNode readSimpleASTNode(const Node::NodeBase *node,
                                  TokenType::TokenType type,
                                  const std::wstring &requireType,
                                  const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE,
                                  std::shared_ptr<ErrorReason> (*check)(const std::wstring_view &str,
                                                                        const TokensView &tokens) = nullptr);

        ASTNode readStringASTNode(const Node::NodeBase *node,
                                  const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE);

        ASTNode readIntegerASTNode(const Node::NodeBase *node,
                                   const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE);

        ASTNode readFloatASTNode(const Node::NodeBase *node,
                                 const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE);

        ASTNode readSymbolASTNode(const Node::NodeBase *node,
                                  const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE);

        ASTNode readUntilWhitespace(const Node::NodeBase *node,
                                    const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE);

        ASTNode readStringOrNumberASTNode(const Node::NodeBase *node,
                                          const ASTNodeId::ASTNodeId &astNodeId = ASTNodeId::NONE);
    };

}// namespace CHelper

#endif//CHELPER_TOKENREADER_H
