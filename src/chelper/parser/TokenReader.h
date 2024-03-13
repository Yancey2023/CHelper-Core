//
// Created by Yancey on 2023/12/15.
//

#pragma once

#ifndef CHELPER_TOKENREADER_H
#define CHELPER_TOKENREADER_H

#include "pch.h"
#include "../lexer/Token.h"
#include "../util/VectorView.h"
#include "ASTNode.h"

#if CHelperDebug == true
#define DEBUG_GET_NODE_BEGIN(node) size_t node##Index = tokenReader.indexStack.size();
#else
#define DEBUG_GET_NODE_BEGIN(node)
#endif

#if CHelperDebug == true
#define DEBUG_GET_NODE_END(node)                          \
    if (node##Index != tokenReader.indexStack.size()) {   \
        Profile::push("TokenReaderIndexError: " +         \
            node->getNodeType()->nodeName + " " +         \
            node->id.value_or("") + " " +                 \
            node->description.value_or(""));              \
        throw Exception::TokenReaderIndexError();         \
    }
#else
#define DEBUG_GET_NODE_END(node)
#endif

namespace CHelper {

    namespace Node {

        class NodeBase;

    } // Node

    class TokenReader {
    public:
        const std::shared_ptr<std::vector<Token>> tokenList;
        size_t index = 0;
        std::vector<size_t> indexStack;

        explicit TokenReader(const std::shared_ptr<std::vector<Token>> &tokenList);

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

        [[nodiscard]] VectorView <Token> collect();

    private:
        ASTNode readSimpleASTNode(const Node::NodeBase *node,
                                  TokenType::TokenType type,
                                  const std::string &requireType,
                                  const std::string &astNodeId,
                                  std::shared_ptr<ErrorReason>(*check)(const std::string &str,
                                                                       const VectorView <Token> &tokens));

    public:
        ASTNode readStringASTNode(const Node::NodeBase *node,
                                  const std::string &astNodeId = "");

        ASTNode readIntegerASTNode(const Node::NodeBase *node,
                                   const std::string &astNodeId = "");

        ASTNode readFloatASTNode(const Node::NodeBase *node,
                                 const std::string &astNodeId = "");

        ASTNode readSymbolASTNode(const Node::NodeBase *node,
                                  const std::string &astNodeId = "");

    };

} // CHelper

#endif //CHELPER_TOKENREADER_H
